#!/usr/bin/env python3
"""Create a small, random subset of the Lichess puzzle database.

The upstream database is ~300 MB compressed and expands to over a gigabyte of
CSV, which is impractical to keep in version control or to bundle with every
release. This script streams the archive and keeps an unbiased random sample
using reservoir sampling, so the committed subset keeps the same difficulty
distribution as the full collection.

Usage:
    scripts/sample_puzzles.py --input puzzles.csv.zst --count 30000 \\
        --output assets/lichess_db_puzzle_sample.csv

``--input`` accepts a ``.zst`` archive (decompressed on the fly, so no
temporary multi-gigabyte file is needed) or a plain ``.csv``.
"""

from __future__ import annotations

import argparse
import random
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Iterator

REPO_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT = REPO_ROOT / "assets" / "lichess_db_puzzle_sample.csv"
DEFAULT_URL = "https://database.lichess.org/lichess_db_puzzle.csv.zst"


def open_stream(path: Path):
    """Yield text lines from a .csv or a .zst compressed .csv."""
    if path.suffix == ".zst":
        if not shutil.which("zstd"):
            sys.exit(
                "error: 'zstd' is required to read a .zst archive.\n"
                "  Arch / CachyOS : sudo pacman -S zstd\n"
                "  Debian/Ubuntu  : sudo apt install zstd\n"
                "  macOS          : brew install zstd"
            )
        process = subprocess.Popen(
            ["zstd", "--decompress", "--stdout", str(path)],
            stdout=subprocess.PIPE,
        )
        assert process.stdout is not None
        stream = (line.decode("utf-8", errors="replace") for line in process.stdout)
        yield from stream
        process.stdout.close()
        process.wait()
    else:
        with path.open("r", encoding="utf-8", errors="replace") as handle:
            yield from handle


def reservoir_sample(lines: Iterator[str], count: int, seed: int) -> list[str]:
    """Keep `count` lines chosen uniformly at random, without buffering all."""
    rng = random.Random(seed)
    sample: list[str] = []
    for index, raw in enumerate(lines):
        line = raw.strip()
        if not line or line.startswith("#") or not valid_puzzle(line):
            continue
        if index < count:
            sample.append(line)
        else:
            # Swap out a random existing element with the current line.
            swap = rng.randrange(index + 1)
            if swap < count:
                sample[swap] = line
    return sample


def valid_puzzle(line: str) -> bool:
    """A usable row needs at least PuzzleId, FEN and Moves."""
    fields = line.split(",")
    return len(fields) >= 3 and bool(fields[1].strip()) and bool(fields[2].strip())


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, help="source .csv or .zst file")
    parser.add_argument(
        "--count", type=int, default=30000, help="number of puzzles to keep"
    )
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--seed", type=int, default=20240614)
    parser.add_argument("--url", default=DEFAULT_URL, help="used when --input is absent")
    args = parser.parse_args()

    tmp_path: Path | None = None
    if args.input:
        source = args.input
        if not source.is_file():
            sys.exit(f"error: {source} does not exist")
    else:
        # Download to a scratch location; never inside the repository.
        tmp_dir = Path.home() / ".cache" / "puzzledchess"
        tmp_dir.mkdir(parents=True, exist_ok=True)
        source = tmp_dir / "lichess_db_puzzle.csv.zst"
        if not source.is_file():
            print(f"==> Downloading {args.url}", flush=True)
            tmp_path = source
            subprocess.run(["curl", "--fail", "--location", "--progress-bar",
                            "-o", str(source), args.url], check=True)

    print(f"==> Sampling {args.count} puzzles from {source}", flush=True)
    sample = reservoir_sample(open_stream(source), args.count, args.seed)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8", newline="\n") as handle:
        for line in sample:
            handle.write(line + "\n")

    if tmp_path is not None:
        tmp_path.unlink(missing_ok=True)

    size_mb = args.output.stat().st_size / (1024 * 1024)
    print(f"==> Wrote {len(sample)} puzzles to {args.output} ({size_mb:.1f} MB)")


if __name__ == "__main__":
    main()