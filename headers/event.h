#pragma once

// ===========================================================================
//  Event / arcade mode
//
//  Turns the puzzle trainer into something a walk-up stranger can understand
//  and enjoy without anyone explaining it:
//
//    ATTRACT  -> title, how to play, reward table and best scores
//    PLAYING  -> one short timed round, solve as many puzzles as you can
//    RESULTS  -> puzzles solved, score, and how many "revolves" the run earned
//
//  Every screen returns to the attract screen by itself, so the machine always
//  resets for the next player. Nothing here replaces the original game: set
//  kEnabled to false and the One Shot / Timed / No Limit menu comes back.
//
//  In-game text is ASCII only - raylib's built-in font has no other glyphs.
// ===========================================================================

#include "raylib.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace event {

// ---------------------------------------------------------------------------
// Tunables - these are the knobs to turn for your event.
// ---------------------------------------------------------------------------

inline constexpr bool kEnabled = true;

// How long one round lasts. The existing 60 second Timed mode is a good
// default for a queue.
inline constexpr float kRoundSeconds = 60.0f;

// Results screen holds this long, then returns to attract on its own.
inline constexpr float kResultsSeconds = 15.0f;

// Pause on a screen after a transition so one keypress cannot skip two screens.
inline constexpr float kInputLockSeconds = 0.6f;

// Where best scores are remembered. The directory only has to be writable; if
// it is not, scores simply stay in memory for this session.
inline const char *const kScoreFile = "puzzledchess_scores.txt";

inline constexpr int kMaxScores = 5;

// ---------------------------------------------------------------------------
// Rewards
//
// Winning a round means solving puzzles, and the brief was that playing well -
// or nearly playing well - should earn something. Tiers are on puzzles solved
// inside one round.
// ---------------------------------------------------------------------------

struct Reward {
  int revolves = 0;
  const char *headline = "KEEP GOING!";
  const char *detail = "";
  Color accent = LIGHTGRAY;
};

inline Reward EvaluateReward(int solved) {
  Reward r;
  if (solved >= 10) {
    r.revolves = 5;
    r.headline = "OUTSTANDING!";
    r.detail = "Ten or more puzzles solved.";
    r.accent = GOLD;
  } else if (solved >= 7) {
    r.revolves = 4;
    r.headline = "BRILLIANT!";
    r.detail = "Seven or more puzzles solved.";
    r.accent = YELLOW;
  } else if (solved >= 4) {
    r.revolves = 3;
    r.headline = "STRONG RUN!";
    r.detail = "Four or more puzzles solved.";
    r.accent = SKYBLUE;
  } else if (solved >= 2) {
    r.revolves = 2;
    r.headline = "NICE WORK!";
    r.detail = "Two or more puzzles solved.";
    r.accent = LIME;
  } else if (solved >= 1) {
    r.revolves = 1;
    r.headline = "SOLVED ONE!";
    r.detail = "At least one puzzle solved.";
    r.accent = GREEN;
  } else {
    r.revolves = 0;
    r.headline = "GIVE IT ANOTHER GO!";
    r.detail = "No puzzles solved yet.";
    r.accent = LIGHTGRAY;
  }
  return r;
}

// ---------------------------------------------------------------------------
// Best scores
// ---------------------------------------------------------------------------

class Leaderboard {
  std::vector<int> scores;

public:
  void Load(const char *path) {
    std::ifstream in(path);
    int s = 0;
    while (in >> s)
      scores.push_back(s);
    Sort();
  }

  void Save(const char *path) const {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open())
      return;  // read-only install directory: scores stay for this session
    for (int s : scores)
      out << s << "\n";
  }

  bool Submit(int score) {
    scores.push_back(score);
    Sort();
    if ((int)scores.size() > kMaxScores)
      scores.resize(kMaxScores);
    return Best() == score;
  }

  int Best() const { return scores.empty() ? 0 : scores.front(); }

  const std::vector<int> &Entries() const { return scores; }

private:
  void Sort() {
    std::sort(scores.begin(), scores.end(), std::greater<int>());
  }
};

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------

enum class Screen { Attract, Playing, Results };

// What changed since the last Tick(), so the game knows when to reset itself.
enum class Change { None, StartRound, FinishRound };

class Session {
  double enteredScreen = 0.0;
  double lastInputAt = -999.0;

public:
  Screen screen = Screen::Attract;
  float roundLeft = kRoundSeconds;
  float resultsLeft = kResultsSeconds;
  Leaderboard board;

  // Filled in by the game when a round ends, read by the results screen.
  int solved = 0;
  int finalScore = 0;
  Reward reward;
  bool newBest = false;

  void Begin() {
    enteredScreen = GetTime();
    lastInputAt = GetTime();
    screen = Screen::Attract;
    board.Load(kScoreFile);
  }

  // Any key or click counts as "I want to play".
  static bool AnyInput() {
    return GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  }

  bool InputLocked() const {
    return GetTime() - lastInputAt < kInputLockSeconds;
  }

  // Call once per frame before drawing. Returns what the game must react to.
  // The solved count and score come from the game, which is the only thing
  // that knows them.
  Change Tick(int solvedNow, int scoreNow) {
    if (screen == Screen::Attract) {
      if (AnyInput() && !InputLocked()) {
        lastInputAt = GetTime();
        screen = Screen::Playing;
        roundLeft = kRoundSeconds;
        enteredScreen = GetTime();
        return Change::StartRound;
      }
      return Change::None;
    }

    if (screen == Screen::Playing) {
      if (AnyInput())
        lastInputAt = GetTime();
      roundLeft -= GetFrameTime();
      if (roundLeft <= 0.0f) {
        roundLeft = 0.0f;
        Finish(solvedNow, scoreNow);
        return Change::FinishRound;
      }
      return Change::None;
    }

    // Results
    resultsLeft -= GetFrameTime();
    bool wantsOut = resultsLeft <= 0.0f ||
                    (AnyInput() && !InputLocked() &&
                     GetTime() - enteredScreen > 1.0f);
    if (wantsOut) {
      lastInputAt = GetTime();
      screen = Screen::Attract;
      enteredScreen = GetTime();
    }
    return Change::None;
  }

  void Finish(int solvedNow, int scoreNow) {
    solved = solvedNow;
    finalScore = scoreNow;
    reward = EvaluateReward(solvedNow);
    newBest = board.Submit(scoreNow);
    board.Save(kScoreFile);
    resultsLeft = kResultsSeconds;
    screen = Screen::Results;
    enteredScreen = GetTime();
  }
};

// The one session the game loop and the screens share.
inline Session session;

// raylib's Color leaves alpha at 0 when you brace-initialise only three
// channels, which silently draws nothing. Always go through this instead.
inline Color RGB(unsigned char r, unsigned char g, unsigned char b,
                 unsigned char a = 255) {
  return Color{r, g, b, a};
}

// ---------------------------------------------------------------------------
// Drawing helpers
// ---------------------------------------------------------------------------

inline void DrawCenteredText(const char *text, int centerX, int y, int size,
                             Color color) {
  int w = MeasureText(text, size);
  DrawText(text, centerX - w / 2, y, size, color);
}

// A soft drop shadow keeps light text readable over the board.
inline void DrawCenteredTextShadow(const char *text, int centerX, int y,
                                   int size, Color color) {
  int w = MeasureText(text, size);
  int x = centerX - w / 2;
  DrawText(text, x + 3, y + 3, size, Color{0, 0, 0, 160});
  DrawText(text, x, y, size, color);
}

inline void Panel(int x, int y, int w, int h, float alpha = 0.94f) {
  DrawRectangleRec(Rectangle{(float)x, (float)y, (float)w, (float)h},
                   Color{8, 8, 20, (unsigned char)(255 * alpha)});
  DrawRectangleLinesEx(Rectangle{(float)x, (float)y, (float)w, (float)h}, 3,
                      Color{255, 255, 255, 45});
}

float Pulse(float speed = 3.0f) {
  return 0.5f + 0.5f * sinf((float)GetTime() * speed);
}

}  // namespace event