#!/usr/bin/env bash
#
# Download the full Lichess open puzzle database.
#
# A random 30,000 puzzle subset is committed at assets/lichess_db_puzzle.csv so
# the game is playable straight after a clone. This script fetches the complete
# collection (~4 million puzzles, 300 MB compressed) alongside it; run the game
# with PUZZLEDCHESS_PUZZLES to point at the larger file:
#
#   PUZZLEDCHESS_PUZZLES=./assets/lichess_db_puzzle_full.csv ./PuzzledChess
#
# To switch over permanently, just copy the file over the active one:
#
#   cp assets/lichess_db_puzzle_full.csv assets/lichess_db_puzzle.csv
#
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

URL="${LICHESS_PUZZLE_URL:-https://database.lichess.org/lichess_db_puzzle.csv.zst}"
TARGET="assets/lichess_db_puzzle_full.csv"

if ! command -v zstd >/dev/null 2>&1; then
    cat >&2 <<'MSG'
error: 'zstd' is required to decompress the database.

  Arch / CachyOS : sudo pacman -S zstd
  Debian/Ubuntu  : sudo apt install zstd
  macOS          : brew install zstd
MSG
    exit 1
fi

if [[ -f "$TARGET" ]]; then
    echo "==> $TARGET already exists ($(wc -l < "$TARGET") puzzles)"
    echo "    delete it first if you want a fresh copy"
    exit 0
fi

echo "==> Downloading $URL"
curl --fail --location --progress-bar -o "$TARGET.zst" "$URL"

echo "==> Decompressing to $TARGET"
zstd --decompress --stdout "$TARGET.zst" > "$TARGET"
rm -f "$TARGET.zst"

echo "==> Done: $(wc -l < "$TARGET") puzzles at $TARGET"
echo
echo "Play with the full database using:"
echo "  PUZZLEDCHESS_PUZZLES=./$TARGET ./PuzzledChess"