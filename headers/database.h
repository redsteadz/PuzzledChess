#pragma once

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// ---------------------------------------------------------------------------
// Puzzle database access
//
// Puzzles come from the Lichess open puzzle database, a CSV with the columns
//   PuzzleId,FEN,Moves,Rating,RatingDeviation,Popularity,NbPlays,GameUrl,ImageUrls
// so column 1 is the starting position and column 2 is the expected solution
// line (space separated UCI moves such as "e2e4 e7e5 g1f3").
//
// The database is read into memory once, on first use. The original
// implementation re-scanned the file from disk for every puzzle, which is fine
// for a toy file but needlessly slow once the dataset grows.
// ---------------------------------------------------------------------------

class Filer {
public:
  // Path of the bundled dataset, overridable with the PUZZLEDCHESS_PUZZLES
  // environment variable (useful for testing against a different subset).
  static string DatabasePath() {
    const char *override = getenv("PUZZLEDCHESS_PUZZLES");
    if (override != nullptr && override[0] != '\0')
      return string(override);
    return "./assets/lichess_db_puzzle.csv";
  }

  // True once at least one puzzle line has been loaded successfully.
  static bool Loaded() {
    EnsureLoaded();
    return !lines.empty();
  }

  // Returns a random non-empty puzzle line, or "" if the database is missing.
  static string ReadRandomLineFromFile() {
    EnsureLoaded();
    if (lines.empty())
      return "";

    static bool seeded = false;
    if (!seeded) {
      std::srand(static_cast<unsigned int>(std::time(nullptr)));
      seeded = true;
    }

    // Retry a couple of times: some lines in the upstream database are
    // malformed, and an empty or header line would be unusable.
    for (int attempt = 0; attempt < 8; attempt++) {
      size_t index = static_cast<size_t>(std::rand()) % lines.size();
      const string &line = lines[index];
      if (!line.empty())
        return line;
    }
    return lines.front();
  }

  // Splits a random puzzle line into { FEN, solution moves }.
  // Returns {"", ""} when the database is unavailable or the line is malformed,
  // so callers never index into an empty vector.
  static pair<string, string> ParseLine() {
    string line = ReadRandomLineFromFile();
    if (line.empty())
      return make_pair(string(""), string(""));

    vector<string> pieces;
    size_t pos = 0;
    while ((pos = line.find(',')) != string::npos) {
      pieces.push_back(line.substr(0, pos));
      line.erase(0, pos + 1);
    }
    pieces.push_back(line);

    // Need at least PuzzleId,FEN,Moves
    if (pieces.size() < 3 || pieces[1].empty() || pieces[2].empty())
      return make_pair(string(""), string(""));

    return make_pair(pieces[1], pieces[2]);
  }

private:
  static vector<string> lines;
  static bool loaded;

  static void EnsureLoaded() {
    if (loaded)
      return;
    loaded = true;

    ifstream file(DatabasePath());
    if (!file.is_open()) {
      cerr << "Error: Unable to open puzzle database " << DatabasePath()
           << endl;
      cerr << "Expected a Lichess puzzle CSV at ./assets/lichess_db_puzzle.csv"
           << endl;
      cerr << "Run scripts/fetch_puzzles.sh to download one." << endl;
      return;
    }

    string line;
    while (std::getline(file, line)) {
      // Strip a UTF-8 BOM and trailing CR so the first column parses cleanly
      // on every platform.
      if (!line.empty() && line.back() == '\r')
        line.pop_back();
      if (line.rfind("\xEF\xBB\xBF", 0) == 0)
        line.erase(0, 3);
      if (line.empty() || line[0] == '#')
        continue;
      lines.push_back(line);
    }

    if (lines.empty())
      cerr << "Error: puzzle database " << DatabasePath() << " is empty."
           << endl;
  }
};

inline vector<string> Filer::lines;
inline bool Filer::loaded = false;