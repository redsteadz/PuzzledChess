# Event mode

This branch turns both games into arcade cabinets for an event: a stranger
walks up to a running screen, understands it without being told, plays one
short round, and sees what they earned.

```
ATTRACT   the game is already running and explaining itself
   |      any key or click
   v
PLAYING   one short timed round, countdown on screen
   |      time runs out, board cleared, or lives run out
   v
RESULTS   what you scored and how many revolves you earned
   |      any key, or 15 seconds
   +------> back to ATTRACT, ready for the next person
```

The machine always resets itself, so nobody has to close the game between
players and nobody can leave it in a broken state.

---

## Quick start

```bash
git clone https://github.com/redsteadz/PuzzledChess.git
cd PuzzledChess
git checkout event-arcade-mode

# Install raylib 5.x, then:
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cmake --install build --prefix dist

cd dist && ./PuzzledChess
```

PACMAN is the same, on its own `event-arcade-mode` branch.

For a machine that cannot be left unattended, put the game folder somewhere
writable, because it saves best scores next to the executable.

---

## Tuning it for your event

Everything worth changing sits at the top of `headers/event.h`.

| Setting | Default | What it does |
| --- | --- | --- |
| `kRoundSeconds` | 90 (PACMAN), 60 (Chess) | How long one round lasts |
| `kResultsSeconds` | 15 | How long the payout screen stays up |
| `kInputLockSeconds` | 0.6 | Stops one keypress skipping two screens |
| `kScoreFile` | `puzzledchess_scores.txt` | Where best scores are stored |
| `kEnabled` | `true` | Set `false` to get the original game back |

### The reward table

This is the part to change first if the payout structure is different. It is a
plain function in `headers/event.h`.

**PACMAN** - `EvaluateReward(percentCleared, clearedBoard)`:

| Board eaten | Revolves |
| --- | --- |
| Cleared the whole board | 5 |
| 90% or more | 3 |
| 70% or more | 2 |
| 40% or more | 1 |

**PuzzledChess** - `EvaluateReward(solved)`, on puzzles solved in one round:

| Puzzles solved | Revolves |
| --- | --- |
| 10 or more | 5 |
| 7 or more | 4 |
| 4 or more | 3 |
| 2 or more | 2 |
| 1 or more | 1 |

Both tables pay out for getting close, not only for winning, so a short or
unlucky run still earns something. Edit the numbers and the edit changes the
attract screen too: the attract screen reads the same table, so players can see
what they are playing for before they start.

---

## What changed for players

**PACMAN**

- A title screen with the goal, the controls and the reward table
- Arrow keys work as well as WASD, which is what most people try first
- A visible countdown, lives as pips, and a progress bar for how much of the
  board is gone, so "getting close" is visible while playing
- The win condition is fixed. It used to compare the score with `==` against
  3730, which is every pellet with no ghosts eaten, so eating a single ghost
  meant the number went straight past the target and the win screen never
  appeared
- Rounds end when the timer or the lives run out
- The map editor is hidden, so a stranger clicking around cannot end up editing
  or saving a map

**PuzzledChess**

- The three-mode menu becomes one timed round, with a three-step how-to on the
  title screen
- Pieces the player may move are tinted blue. Which colour that is depends on
  the position, so working it out by trial and error was the fastest way to
  lose someone's interest
- A rejected move says "NOT THE MOVE WE WANTED" and a completed puzzle says
  "SOLVED!", so a click that does nothing no longer looks like a broken mouse
- The Right Arrow solve button is disabled; it handed out free points
- The score, the number solved and the countdown sit below the board, so
  nothing covers a playable square

---

## Running it unattended

- Best scores are written to the working directory. If the folder is read-only
  the game still works, scores just do not persist between restarts.
- Escape still closes the window, which is the normal way to shut down.
- The round timer guarantees the next player is never locked out of a
  half-finished game.
- No network, no accounts, no external services.