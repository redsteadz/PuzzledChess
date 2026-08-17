# PuzzledChess

![PuzzledChess screenshot](https://github.com/redsteadz/PuzzledChess/assets/86804632/ad55a329-0626-449a-a0f9-cc1f3f9b2616)

PuzzledChess is a C++ and raylib chess-puzzle application. It loads random positions and solution lines from a bundled copy of the Lichess puzzle database, then verifies the player's moves against the expected continuation.

## Features

- Random puzzle selection from the bundled Lichess CSV data
- Click-to-select board interaction with suggested destination squares
- Animated player and opponent moves with sound feedback
- Score rewards based on solve speed
- Three play modes:
  - **One Shot:** an incorrect move ends the run and returns to the menu
  - **Timed:** solve as many puzzles as possible in a 60-second session
  - **No Limit:** continue without a countdown
- `Right Arrow` assistance that plays the next expected move

## Build and run

You need a C++ compiler, CMake, and a raylib installation discoverable by CMake.

~~~bash
git clone https://github.com/redsteadz/PuzzledChess.git
cd PuzzledChess
cmake -S . -B build
cmake --build build
cd build
./Chess
~~~

CMake copies `assets/`, including the puzzle dataset, into the build directory.

## Data and license

Puzzle positions come from the [Lichess open puzzle database](https://database.lichess.org/#puzzles). This repository is licensed under the [GNU AGPL v3](LICENSE).
