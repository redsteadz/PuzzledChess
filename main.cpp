#include "cmath"
#include "headers/animate.h"
#include "headers/console.h"
#include "headers/constant.h"
#include "headers/database.h"
#include "headers/event.h"
#include "headers/gui.h"
#include "headers/sound.h"
#include "map"
#include "raylib.h"
#include <bits/stdc++.h>
#include <ios>
#include <iostream>
#include <string>
#include <utility>

// raygui and gui_menu are vendored third-party headers (upstream raygui
// sources). They are included here with warnings relaxed so the project can
// keep a clean -Wall build without patching upstream code.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wenum-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define RAYGUI_IMPLEMENTATION
#include "headers/raygui.h"
#define GUI_MENU_IMPLEMENTATION
#include "headers/gui_menu.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

using namespace std;

string board[8][8];
Color colorBoard[8][8];
vector<vector<Color>> selectedBoard(8, vector<Color>(8));
int score = 0;
string score_str = "0";
int menu_time = 0;
int max_score = 0;

// Event mode bookkeeping: how many puzzles the current round has solved, and
// a short-lived banner so a click that does nothing still feels like it was
// registered.
int puzzles_solved = 0;
string feedback_text = "";
double feedback_until = 0.0;

static void Say(const char *text, double seconds = 1.4) {
  feedback_text = text;
  feedback_until = GetTime() + seconds;
}

static bool FeedbackActive() { return GetTime() < feedback_until; }

// Need to add empassant, Castling, Check, checkmate, oawn promotion


string UpdateTime(int time_f) {
  int hours = time_f / 3600;
  int minutes = (time_f % 3600) / 60;
  int seconds = time_f % 60;

  ostringstream oss;
  oss << setfill('0') << setw(2) << hours << ":" << setfill('0')
      << setw(2) << minutes << ":" << setfill('0') << setw(2)
      << seconds;

  return oss.str();
}

class Piece {
  string name;

public:
  Piece(string name) { this->name = name; }
  void virtual ViewMoves() = 0;
};

static bool inBounds(Vector2 pos) {
  if (pos.x < 8 && pos.y < 8 && pos.x > -1 && pos.y > -1)
    return true;
  return false;
}

class HandlePiece {
public:
  static void Pawn(string name, Vector2 pos) {
    char enemy = name[0] == 'w' ? 'b' : 'w';
    bool homeRow = ((name[0] == 'w') ? 7 - pos.y : pos.y) == 1;
    int mag = homeRow ? 2 : 1;
    // Direction
    int dir = name[0] == 'w' ? 1 : -1;
    // If dir + color the board from pos.y += dir for mag
    for (int i = 1; i <= mag; i++) {
      if (inBounds(Vector2{pos.x, pos.y - i * dir})) {
        if (board[(int)pos.y - i * dir][(int)pos.x] != "")
          break;
        // selectedBoard[(int)pos.y - i * dir][(int)pos.x] = BLUE;
        ExpandAnimation *anim = new ExpandAnimation(
            {(float)pos.x, (float)pos.y - i * dir}, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
      }
    }

    // Check 1 diagonal
    vector<int> dy = {-1, -1};
    vector<int> dx = {-1, +1};
    for (int y : dy) {
      for (int x : dx) {
        int valx = pos.x + x;
        int valy = pos.y + dir * y;
        if (valx > -1 && valx < 8 && valy > -1 && valy < 8)
          if (board[valy][valx][0] == enemy) {
            // selectedBoard[valy][valx] = RED;
            ExpandAnimation *anim = new ExpandAnimation(
                {(float)valx, (float)valy}, 0.2, &selectedBoard, RED);
            AnimationManager::addAnimation(anim);
          }
      }
    }
  }

  static void Bishop(string name, Vector2 pos) {
    char enemy = name[0] == 'w' ? 'b' : 'w';
    char friendly = name[0] == 'w' ? 'w' : 'b';
    // Highlight all the diagonals if in bounds
    // Top Right
    for (int i = 1; i < 8; i++) {
      Vector2 newPos = {pos.x + i, pos.y - i};
      if (inBounds(Vector2{pos.x + i, pos.y - i})) {
        // If it is a piece of the same color break
        if (board[(int)newPos.y][(int)newPos.x][0] == friendly)
          break;
        Color color =
            board[(int)pos.y - i][(int)pos.x + i][0] == enemy ? RED : BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation(newPos, 0.2, &selectedBoard, color);
        AnimationManager::addAnimation(anim);
        if (board[(int)pos.y - i][(int)pos.x + i] != "")
          break;
      }
    }
    // Top Left
    for (int i = 1; i < 8; i++) {
      Vector2 newPos = {pos.x - i, pos.y - i};
      if (inBounds(Vector2{pos.x - i, pos.y - i})) {
        if (board[(int)newPos.y][(int)newPos.x][0] == friendly)
          break;

        Color color =
            board[(int)pos.y - i][(int)pos.x - i][0] == enemy ? RED : BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation(newPos, 0.2, &selectedBoard, color);
        AnimationManager::addAnimation(anim);
        if (board[(int)pos.y - i][(int)pos.x - i] != "")
          break;
      }
    }
    // Bottom Right
    for (int i = 1; i < 8; i++) {
      Vector2 newPos = {pos.x + i, pos.y + i};
      if (inBounds(newPos)) {
        if (board[(int)newPos.y][(int)newPos.x][0] == friendly)
          break;

        Color color =
            board[(int)newPos.y][(int)newPos.x][0] == enemy ? RED : BLUE;
        // selectedBoard[(int)newPos.y][(int)newPos.x] = color;
        ExpandAnimation *anim =
            new ExpandAnimation(newPos, 0.2, &selectedBoard, color);
        AnimationManager::addAnimation(anim);
        if (board[(int)newPos.y][(int)newPos.x] != "")
          break;
      }
    }
    // Bottom Left
    for (int i = 1; i < 8; i++) {
      Vector2 newPos = {pos.x - i, pos.y + i};
      if (inBounds(Vector2{pos.x - i, pos.y + i})) {
        if (board[(int)newPos.y][(int)newPos.x][0] == friendly)
          break;

        Color color =
            board[(int)pos.y + i][(int)pos.x - i][0] == enemy ? RED : BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation(newPos, 0.2, &selectedBoard, color);
        AnimationManager::addAnimation(anim);
        if (board[(int)pos.y + i][(int)pos.x - i] != "")
          break;
      }
    }
  }

  static void Rook(string name, Vector2 pos) {
    // Highlight all the rows if in bounds
    char enemy = name[0] == 'w' ? 'b' : 'w';
    // Go Horizontal Right
    for (int i = pos.x + 1; i < 8; i++) {
      if (board[(int)pos.y][i] == "") {
        // selectedBoard[(int)pos.y][i] = BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation({(float)i, pos.y}, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
      } else if (board[(int)pos.y][i][0] == enemy) {
        // selectedBoard[(int)pos.y][i] = RED;
        ExpandAnimation *anim =
            new ExpandAnimation({(float)i, pos.y}, 0.2, &selectedBoard, RED);
        AnimationManager::addAnimation(anim);
        break;
      } else {
        break;
      }
    }

    // Go Horizontal Left
    for (int i = pos.x - 1; i > -1; i--) {
      if (board[(int)pos.y][i] == "") {
        // selectedBoard[(int)pos.y][i] = BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation({(float)i, pos.y}, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
      } else if (board[(int)pos.y][i][0] == enemy) {
        // selectedBoard[(int)pos.y][i] = RED;
        ExpandAnimation *anim =
            new ExpandAnimation({(float)i, pos.y}, 0.2, &selectedBoard, RED);
        AnimationManager::addAnimation(anim);
        break;
      } else {
        break;
      }
    }

    // Go Vertical Up
    for (int i = pos.y - 1; i > -1; i--) {
      if (board[i][(int)pos.x] == "") {
        // selectedBoard[i][(int)pos.x] = BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation({pos.x, (float)i}, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
      } else if (board[i][(int)pos.x][0] == enemy) {
        // selectedBoard[i][(int)pos.x] = RED;
        ExpandAnimation *anim =
            new ExpandAnimation({pos.x, (float)i}, 0.2, &selectedBoard, RED);
        AnimationManager::addAnimation(anim);
        break;
      } else {
        break;
      }
    }

    // Go Vertical Down
    for (int i = pos.y + 1; i < 8; i++) {
      if (board[i][(int)pos.x] == "") {
        // selectedBoard[i][(int)pos.x] = BLUE;
        ExpandAnimation *anim =
            new ExpandAnimation({pos.x, (float)i}, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
      } else if (board[i][(int)pos.x][0] == enemy) {
        // selectedBoard[i][(int)pos.x] = RED;
        ExpandAnimation *anim =
            new ExpandAnimation({pos.x, (float)i}, 0.2, &selectedBoard, RED);
        AnimationManager::addAnimation(anim);
        break;
      } else {
        break;
      }
    }
  }

  void static Knight(string name, Vector2 pos) {
    // Highlight all the rows if in bounds
    char enemy = name[0] == 'w' ? 'b' : 'w';

    int delrow[8] = {+2, -1, +1, -2, +2, +1, -1, -2};
    int delcol[8] = {+1, +2, +2, +1, -1, -2, -2, -1};
    for (int i = 0; i < 8; i++) {
      Vector2 newP = {pos.x + delrow[i], pos.y + delcol[i]};
      if (inBounds(newP)) {
        if (board[(int)newP.y][(int)newP.x] == "") {
          // selectedBoard[(int)newP.y][(int)newP.x] = BLUE;
          ExpandAnimation *anim =
              new ExpandAnimation(newP, 0.2, &selectedBoard, BLUE);
          AnimationManager::addAnimation(anim);
        }
        if (board[(int)newP.y][(int)newP.x][0] == enemy) {
          // selectedBoard[(int)newP.y][(int)newP.x] = RED;
          ExpandAnimation *anim =
              new ExpandAnimation(newP, 0.2, &selectedBoard, RED);
          AnimationManager::addAnimation(anim);
        }
      }
    }
  }

  static void King(string name, Vector2 pos) {
    char enemy = name[0] == 'w' ? 'b' : 'w';
    int dx[] = {0, 1, -1};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int x = pos.x + dx[i];
        int y = pos.y + dx[j];
        if (x < 8 && y < 8 && x > -1 && y > -1) {
          if (board[y][x][0] == enemy) {
            // selectedBoard[y][x] = RED;
            ExpandAnimation *anim = new ExpandAnimation(
                {(float)x, (float)y}, 0.2, &selectedBoard, RED);
            AnimationManager::addAnimation(anim);
          }
          if (board[y][x] == "") {
            // selectedBoard[y][x] = BLUE;
            ExpandAnimation *anim = new ExpandAnimation(
                {(float)x, (float)y}, 0.2, &selectedBoard, BLUE);
            AnimationManager::addAnimation(anim);
          }
        }
      }
    }
  }

  static void Queen(string name, Vector2 pos) {
    Bishop(name, pos);
    Rook(name, pos);
  }

  static bool PieceEval(string name, Vector2 pos) {
    if (name.length() < 2)
      return 0;
    char piece = name[1];
    switch (piece) {
    case 'P':
      Pawn(name, pos);
      break;
    case 'N':
      Knight(name, pos);
      break;
    case 'B':
      Bishop(name, pos);
      break;
    case 'R':
      Rook(name, pos);
      break;
    case 'Q':
      Queen(name, pos);
      break;
    case 'K':
      King(name, pos);
      break;
    default:
      cout << "Piece not found";
      break;
    }
    return true;
  }

private:
};

class Board {
  map<string, Texture2D> texs;
  Vector2 start = {-1, -1}, end = {-1, -1};
  double timeStarted;
  bool set = false;
  vector<pair<Vector2, Vector2>> moves;
  int moveCount = 0;
  // 0 is NPC 1 is player
  char playerColor = 'w';
  // bool enPassant[2];
  // bool whiteKing[2];
  // King & Queen
  bool castleB[2];
  bool castleW[2];

  GAME_STATE gameState = MENU;

public:
  void Reset() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? BLACK : WHITE;
        colorBoard[i][j] = squareColor;
        board[i][j] = "";
        selectedBoard[i][j] = WHITE;
      }
    moves.erase(moves.begin(), moves.end());
    moveCount = 0;
    timeStarted = GetTime();
    set = false;
  }
  void setGameState(GAME_STATE state) { gameState = state; }
  Board() {
    Reset();
    // Load textures for each pieces
    string pieces[] = {"wP", "wN", "wB", "wR", "wQ", "wK",
                       "bP", "bN", "bB", "bR", "bQ", "bK"};
    for (string name : pieces) {
      string texName = "./assets/" + name + ".png";
      texs[name] = LoadTexture(texName.c_str());
    }
    castleB[0] = castleB[1] = false;
    castleW[0] = castleW[1] = false;
    NewGame();
  }
  void ParseMoves(string moveSet) {
    // Break the string into pieces
    string move;
    vector<string> movesS = breakBySpace(moveSet);

    for (string m : movesS) {
      Vector2 start = {((float)m[0] - 'a'), 7 - ((float)m[1] - '1')};
      Vector2 end = {(float)m[2] - 'a', 7 - ((float)m[3] - '1')};
      pair<Vector2, Vector2> p = {start, end};
      moves.push_back(p);
    }
  }
  void setBoard(string FEN) {
    Reset();
    int i = 0, j = 0;
    for (char c : FEN) {
      if (c == ' ') {
        break;
      }
      if (c == '/') {
        i++;
        j = 0;
        continue;
      }
      if (c <= '9' && c >= '1') {
        j += (c - '0');
      }

      if (c != '/' && (c > '9' || c < '1')) {
        // This character represents a piece
        string tex_name = "";
        if (c <= 'z' && c >= 'a') {
          tex_name += "b";
          c = c - 'a' + 'A';
        } else
          tex_name += "w";

        tex_name += c;
        board[i][j] = tex_name;
        j++;
      }
    }

    string FEN_add =
        FEN.substr(FEN.find(' ') + 1, FEN.length() - FEN.find(' '));
    vector<string> FEN_addPiece = breakBySpace(FEN_add);
    playerColor = (FEN_addPiece[0][0] == 'b') ? 'w' : 'b';
    // Castling setting
    for (char c : FEN_addPiece[1]) {
      switch (c) {
      case 'k':
        castleB[0] = true;
        break;
      case 'q':
        castleB[1] = true;
        break;
      case 'K':
        castleW[0] = true;
        break;
      case 'Q':
        castleW[1] = true;
        break;
      }
    }
  }
  void Draw() {
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        // Draw the square with color from colorBoard
        DrawRectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE,
                      SQUARE_SIZE, colorBoard[y][x]);
      }
    }
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        // Draw colored square on selectedBoard if the color is not WHITE
        if (!CompareColor(selectedBoard[y][x], WHITE)) {
          DrawRectangle(x * SQUARE_SIZE + gap, y * SQUARE_SIZE + gap,
                        SQUARE_SIZE - gap * 2, SQUARE_SIZE - gap * 2,
                        selectedBoard[y][x]);
        }
      }
    }
    AnimationManager::Draw();
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        // Draw the piece if the board cell is not empty
        if (board[y][x] != "") {
          DrawTexturePro(texs[board[y][x]], Rectangle{0, 0, 16, 16},
                         Rectangle{(float)x * SQUARE_SIZE,
                                   (float)y * SQUARE_SIZE, SQUARE_SIZE,
                                   SQUARE_SIZE},
                         Vector2{0, 0}, 0, WHITE);
          // Tint the pieces the player is allowed to move. Which colour that
          // is depends on the position, and picking one up by trial and error
          // is the fastest way to lose a stranger's interest.
          if (board[y][x][0] == playerColor) {
            DrawRectangle(x * SQUARE_SIZE + gap, y * SQUARE_SIZE + gap,
                          SQUARE_SIZE - gap * 2, SQUARE_SIZE - gap * 2,
                          event::RGB(90, 170, 255, 90));
          }
        }
      }
    }
  }
  void HandleClick() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 pos = MousePos();
      cout << pos.y << " " << pos.x << endl;
      // Find the start position
      // cout << pos.y << " " << pos.x << endl;
      if (set) {
        // Means that it has selected a piece, now verify this position to be
        // 1. Blue
        // 2. That of an enemy piece / empty
        // cout << "START: " << start.y << " " << start.x << endl;
        if (inBounds(pos) && ValidMove(pos) && VerifyMove(pos)) {
          // Make this move
          PlayMove();
          if (moveCount < (int)moves.size())
            EnemyMove();
          else {
            UpdateScore();
            NewGame();
          }
        }
        // cout << ValidMove(pos) << " " << VerifyMove(pos) << endl;
        set = false;
        start = {-1, -1};
        // Verify that the next move was in the moveset
        // then play enemies turn
      }

      ResetBoardColor();
      if (inBounds(pos) && board[(int)pos.y][(int)pos.x][0] == playerColor) {
        set = HandlePiece::PieceEval(board[(int)pos.y][(int)pos.x], pos);
      }
      if (set)
        start = pos;
      else
        start = {-1, -1};
    }
  }

  void PrintMoves() {
    if (moves.size() == 0) {
      return;
    }
    for (pair<Vector2, Vector2> move : moves) {
      cout << "(" << (int)move.first.y << ", " << (int)move.first.x << ") -> ("
           << (int)move.second.y << ", " << (int)move.second.x << ")" << endl;
    }
  }
  void EnemyMove() {
    Vector2 enemyStart = moves[moveCount].first;
    Vector2 enemyEnd = moves[moveCount].second;
    board[(int)enemyEnd.y][(int)enemyEnd.x] =
        board[(int)enemyStart.y][(int)enemyStart.x];
    board[(int)enemyStart.y][(int)enemyStart.x] = "";
    MovePieceAnim *move = new MovePieceAnim(
        &texs[board[(int)enemyEnd.y][(int)enemyEnd.x]], enemyStart, enemyEnd);
    AnimationManager::addAnimation(move);
    PlaySound(SoundMap::soundMap[Move]);
    moveCount++;
  }

  bool VerifyMove(Vector2 pos) {
    Vector2 correctStart = moves[moveCount].first;
    Vector2 correctEnd = moves[moveCount].second;
    if (start.x == correctStart.x && start.y == correctStart.y &&
        pos.x == correctEnd.x && pos.y == correctEnd.y)
      return true;
    // Say so. Without this a rejected move looks exactly like a broken mouse.
    Say("NOT THE MOVE WE WANTED");
    // If one shot mode then the game has ended
    if (gameState == ONE_SHOT) {
      cout << "YOU LOST: " << score << endl;
      NewGame();
      // Reset SCORE
      score = 0;
      score_str = to_string(score);
      timeStarted = GetTime();
      gameState = MENU;
    }
    return false;
  }
  void PlayMove() {
    Vector2 correctStart = moves[moveCount].first;
    Vector2 correctEnd = moves[moveCount].second;
    board[(int)correctEnd.y][(int)correctEnd.x] =
        board[(int)correctStart.y][(int)correctStart.x];
    board[(int)correctStart.y][(int)correctStart.x] = "";
    MovePieceAnim *move =
        new MovePieceAnim(&texs[board[(int)correctEnd.y][(int)correctEnd.x]],
                          correctStart, correctEnd);
    AnimationManager::addAnimation(move);
    PlaySound(SoundMap::soundMap[Move]);
    moveCount++;
  }
  void Cheat() {
    PlayMove();
    if (moveCount < (int)moves.size())
      EnemyMove();
    else {
      UpdateScore();
      NewGame();
    }
  }
  void UpdateScore() {
    double detalTime = GetTime() - timeStarted - menu_time;
    if (detalTime >= 2 * 60)
      score += 100;
    else if (detalTime >= 60)
      score += 200;
    else if (detalTime >= 30)
      score += 300;
    else if (detalTime >= 20)
      score += 400;
    else if (detalTime >= 10)
      score += 500;
    else
      score += 1000;
    // Make an animation for the score
    score_str = to_string(score);
    PlaySound(SoundMap::soundMap[SCORE]);
    puzzles_solved++;
    Say("SOLVED!", 1.2);
    TextAnim *scoreAnim =
        new TextAnim(score_str, Vector2{650, 800}, Vector2{650, 700});
    AnimationManager::addAnimation(scoreAnim);
  }
  void NewGame() {
    pair<string, string> Board_Moves = Filer::ParseLine();
    if (Board_Moves.first.empty() || Board_Moves.second.empty()) {
      // The puzzle database could not be read. Leave the board empty rather
      // than indexing into an empty FEN and crashing.
      cerr << "Error: no usable puzzle could be loaded from "
           << Filer::DatabasePath() << endl;
      return;
    }
    cout << Board_Moves.first << endl;
    cout << Board_Moves.second << endl;
    setBoard(Board_Moves.first);
    ParseMoves(Board_Moves.second);
    cout << "THE MOVES -------------->" << endl;
    PrintMoves();
    EnemyMove();
    timeStarted = GetTime();
  }

private:
  vector<string> breakBySpace(string str) {
    vector<string> tokens;
    istringstream iss(str);
    for (string s; iss >> s;) {
      tokens.push_back(s);
    }
    return tokens;
  }
  void ResetBoardColor() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? BLACK : WHITE;
        colorBoard[i][j] = squareColor;
        selectedBoard[i][j] = WHITE;
      }
    AnimationManager::clearBoard();
  }
  Vector2 MousePos() {
    Vector2 mouse = GetMousePosition();
    Vector2 pos = {floor(mouse.x / SQUARE_SIZE), floor(mouse.y / SQUARE_SIZE)};
    return pos;
  }
  bool ValidMove(Vector2 pos) {
    // cout << "pos: " << pos.y << " " << pos.x << endl;
    if (!CompareColor(selectedBoard[(int)pos.y][(int)pos.x], BLUE) &&
        !CompareColor(selectedBoard[(int)pos.y][(int)pos.x], RED))
      return false;
    // Check if the piece is opp to the start and or empty
    if (board[(int)pos.y][(int)pos.x] == "")
      return true;
    char startPiece = board[(int)start.y][(int)start.x][0];
    char endPiece = board[(int)pos.y][(int)pos.x][0];
    if (startPiece != endPiece)
      return true;
    return false;
  }
  bool CompareColor(Color a, Color b) {
    Vector3 a_V = ColorToHSV(a);
    Vector3 b_V = ColorToHSV(b);
    return a_V.x == b_V.x && a_V.y == b_V.y && a_V.z == b_V.z;
  }
};

unordered_multiset<Animation *> AnimationManager::animations;

map<SoundType, Sound> SoundMap::soundMap;

class Game {
  Board b;
  Label Time;
  Label ScoreLabel;
  string time = "00:00:00";
  SmallButton Button;
  BigButton OneShot;
  BigButton Timed;
  BigButton No_Limit;
  GAME_STATE gameState = MENU;
  Texture2D GuiSheet = LoadTexture("./assets/GUI.png");
  GuiMenuState state = InitGuiMenu();
  Texture2D bg;

public:
  void setState(GAME_STATE state) { gameState = state; }
  Game()
      : Time(Vector2{400 - 48 * 3 / 2, 800}, &time),
        ScoreLabel(Vector2{650, 800}, &score_str),
        Button(Vector2{0, 800}, &GuiSheet),
        OneShot({SCREEN_WIDTH / 2.0 - 48 * 3 / 2.0, 300}, "ONE SHOT", &GuiSheet,
                ONE_SHOT, &gameState),
        Timed({SCREEN_WIDTH / 2.0 - 48 * 3 / 2.0, 400}, "TIMED", &GuiSheet,
              TIMED, &gameState),
        No_Limit({SCREEN_WIDTH / 2.0 - 48 * 3 / 2.0, 500}, "NO LIMIT",
                 &GuiSheet, NO_LIMIT, &gameState) {
    bg = LoadTexture("./assets/background.png");
    SoundMap::Init();
    // Make a function that updates the string time
  }
  void HANDLE_GAME_STATE() {
    string max_score_str = "Max Score: " + to_string(max_score);
    switch (gameState) {
    case MENU:
      // Draw the MENU
      DrawTexture(bg, 0, 0, WHITE);
      DrawText(max_score_str.c_str(),
               SCREEN_WIDTH / 2 - MeasureText(max_score_str.c_str(), 20) / 2,
               100 + 10, 20, WHITE);
      // GuiMenu(&state, gameState);
      OneShot.Draw();
      OneShot.Update();
      Timed.Draw();
      Timed.Update();
      No_Limit.Draw();
      No_Limit.Update();
      break;
    case ONE_SHOT:
      // Draw for ONE_SHOT
      time = UpdateTime((GetTime() - menu_time));
      if (IsKeyPressed(KEY_RIGHT))
        b.Cheat();
      b.Draw();
      Button.Update();
      Time.Draw();
      ScoreLabel.Draw();
      Button.Draw();
      AnimationManager::Update();
      b.HandleClick();
      break;
    case TIMED:
      // Draw for TIMED
      // Timer for 5 minutes
      time = UpdateTime(1 * 60 - (GetTime() - menu_time));
      if (1 * 60 - (GetTime() - menu_time) <= 0) {
        gameState = MENU;
      }
      if (IsKeyPressed(KEY_RIGHT))
        b.Cheat();
      b.Draw();
      Button.Update();
      Time.Draw();
      ScoreLabel.Draw();
      Button.Draw();
      AnimationManager::Update();
      b.HandleClick();
      break;
    case NO_LIMIT:
      time = UpdateTime((GetTime() - menu_time));
      if (IsKeyPressed(KEY_RIGHT))
        b.Cheat();
      b.Draw();
      Button.Update();
      Time.Draw();
      ScoreLabel.Draw();
      Button.Draw();
      AnimationManager::Update();
      b.HandleClick();
      break;
    }
    b.setGameState(gameState);
    if (gameState == MENU)
      menu_time = GetTime();
    max_score = max(max_score, score);
  }
};

// ===========================================================================
//  Event mode: attract screen, HUD and results screen
//
//  These screens sit on top of the original game. The board, the move
//  verification and the scoring are unchanged; this only replaces the menu of
//  three modes that needed explaining with one a walk-up stranger can use
//  unassisted.
// ===========================================================================

namespace event {

// --- Attract ---------------------------------------------------------------

void DrawAttract() {
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{0, 0, 0, 230});

  const int px = 50, py = 40, pw = 700, ph = 770;
  const int cx = px + pw / 2;
  Panel(px, py, pw, ph);

  event::DrawCenteredTextShadow("PUZZLED CHESS", cx, py + 18, 50, SKYBLUE);
  event::DrawCenteredText("Find the best move before the clock runs out.", cx,
                          py + 86, 20, event::RGB(200, 200, 220));
  event::DrawCenteredText("SOLVE AS MANY PUZZLES AS YOU CAN", cx, py + 118, 24,
                          GOLD);

  // How to play. Two clicks is genuinely all it takes, so say exactly that.
  DrawRectangle(px + 50, py + 158, pw - 100, 3, Color{255, 255, 255, 45});
  event::DrawCenteredText("HOW TO PLAY", cx, py + 172, 22, YELLOW);
  event::DrawCenteredText("1.   Click a piece that glows blue", cx, py + 208, 19,
                          WHITE);
  event::DrawCenteredText("2.   Click the square you want it to go", cx,
                          py + 236, 19, WHITE);
  event::DrawCenteredText("3.   Your opponent replies. Keep going.", cx,
                          py + 264, 19, WHITE);

  DrawRectangle(px + 50, py + 296, pw - 100, 3, Color{255, 255, 255, 45});

  // Best scores, so there is a reason to come back for another go.
  event::DrawCenteredText("BEST SCORES", cx, py + 308, 22, YELLOW);
  const std::vector<int> &entries = session.board.Entries();
  if (entries.empty()) {
    event::DrawCenteredText("no scores yet - be the first", cx, py + 350, 19,
                            event::RGB(150, 150, 170));
  } else {
    int row = py + 342;
    int place = 1;
    for (int s : entries) {
      char line[64];
      snprintf(line, sizeof(line), "%d.      %d", place++, s);
      DrawText(line, cx - 100, row, 20, WHITE);
      row += 26;
      if (place > (int)kMaxScores)
        break;
    }
  }

  // Rewards - the reason to get close even without a big score.
  DrawRectangle(px + 50, py + 482, pw - 100, 3, Color{255, 255, 255, 45});
  event::DrawCenteredText("REVOLVES EARNED", cx, py + 494, 22, YELLOW);
  struct Row {
    const char *left;
    int amount;
  };
  static const Row rows[] = {
      {"Solve 10 or more puzzles", 5},
      {"Solve 7 or more", 4},
      {"Solve 4 or more", 3},
      {"Solve 2 or more", 2},
      {"Solve at least 1", 1},
  };
  int ry = py + 528;
  for (const Row &r : rows) {
    char line[80];
    snprintf(line, sizeof(line), "%s  ........  %d", r.left, r.amount);
    DrawText(line, cx - 195, ry, 19, event::RGB(220, 220, 235));
    ry += 27;
  }

  float p = Pulse(4.0f);
  Color prompt = event::RGB(255, 255, (unsigned char)(140 + 115 * p));
  event::DrawCenteredTextShadow("PRESS ANY KEY TO PLAY", cx, py + 668, 34,
                                prompt);
  event::DrawCenteredText("(or click anywhere)", cx, py + 712, 18,
                          event::RGB(140, 140, 165));
}

// --- Heads-up display ------------------------------------------------------
//
// The board fills the window, so the HUD lives in the strip below it rather
// than covering squares the player might need to click.

void DrawHUD() {
  const int barY = SCREEN_HEIGHT - 50;

  bool urgent = session.roundLeft <= 10.0f;
  float p = Pulse(8.0f);
  Color timeColor =
      urgent ? event::RGB(255, (unsigned char)(90 + 80 * p), 90) : WHITE;

  DrawRectangle(0, barY, SCREEN_WIDTH, 50, event::RGB(10, 10, 24));
  DrawRectangle(0, barY, SCREEN_WIDTH, 2,
                urgent ? event::RGB(255, 90, 90) : event::RGB(70, 70, 110));

  char left[64];
  snprintf(left, sizeof(left), "SCORE %d", score);
  DrawText(left, 14, barY + 14, 22, WHITE);

  char mid[64];
  snprintf(mid, sizeof(mid), "SOLVED %d", puzzles_solved);
  DrawText(mid, (SCREEN_WIDTH - MeasureText(mid, 22)) / 2, barY + 14, 22,
           SKYBLUE);

  int secs = (int)ceilf(session.roundLeft);
  char right[32];
  snprintf(right, sizeof(right), "%ds", secs);
  DrawText(right, SCREEN_WIDTH - MeasureText(right, 26) - 16, barY + 11, 26,
           timeColor);

  // A banner over the board for a moment after a click, so nothing ever feels
  // like it silently failed.
  if (FeedbackActive()) {
    int w = MeasureText(feedback_text.c_str(), 30);
    DrawRectangleRec(
        Rectangle{(float)((SCREEN_WIDTH - w) / 2 - 18), 30.0f, (float)(w + 36),
                  52.0f},
        Color{0, 0, 0, 200});
    event::DrawCenteredText(feedback_text.c_str(), SCREEN_WIDTH / 2, 42, 30,
                            GOLD);
  }
}

// --- Results ---------------------------------------------------------------

void DrawResults() {
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{0, 0, 0, 225});

  const int px = 100, py = 70, pw = 600, ph = 710;
  const int cx = px + pw / 2;
  Panel(px, py, pw, ph);

  Color accent = session.reward.accent;
  Color lit = event::RGB(accent.r, accent.g, accent.b);
  event::DrawCenteredTextShadow("TIME UP", cx, py + 22, 40, lit);
  event::DrawCenteredText(session.reward.detail, cx, py + 74, 19,
                          event::RGB(190, 190, 210));

  // The payout, as a badge. This is the bit the event actually cares about.
  int badgeY = py + 190;
  int badgeR = 88;
  DrawCircleV(Vector2{(float)cx, (float)badgeY}, (float)badgeR,
              Color{accent.r, accent.g, accent.b, 60});
  DrawCircleLinesV(Vector2{(float)cx, (float)badgeY}, (float)badgeR, lit);
  char amount[16];
  snprintf(amount, sizeof(amount), "%d", session.reward.revolves);
  DrawText(amount, cx - MeasureText(amount, 72) / 2, badgeY - 42, 72, lit);
  event::DrawCenteredText("REVOLVES", cx, badgeY + badgeR + 2, 21,
                          event::RGB(210, 210, 230));

  DrawRectangle(px + 50, py + 320, pw - 100, 3, Color{255, 255, 255, 45});
  char line[80];
  snprintf(line, sizeof(line), "PUZZLES SOLVED   %d", session.solved);
  DrawText(line, px + 70, py + 342, 22, WHITE);
  snprintf(line, sizeof(line), "SCORE            %d", session.finalScore);
  DrawText(line, px + 70, py + 378, 22, WHITE);
  snprintf(line, sizeof(line), "BEST             %d", session.board.Best());
  DrawText(line, px + 70, py + 414, 22, event::RGB(190, 190, 210));

  if (session.newBest) {
    float sp = Pulse(6.0f);
    event::DrawCenteredText(
        "NEW HIGH SCORE!", cx, py + 462, 28,
        event::RGB(255, 230, (unsigned char)(80 + 100 * sp)));
  }

  int left = (int)ceilf(session.resultsLeft);
  snprintf(line, sizeof(line), "back to the menu in %d", left);
  event::DrawCenteredText(line, cx, py + 518, 19, event::RGB(150, 150, 170));
  float fill = session.resultsLeft / kResultsSeconds;
  DrawRectangleRec(Rectangle{(float)(px + 70), (float)(py + 550),
                             (float)(pw - 140), 10},
                   event::RGB(50, 50, 70));
  DrawRectangleRec(Rectangle{(float)(px + 70), (float)(py + 550),
                             (float)(pw - 140) * fill, 10},
                   lit);

  event::DrawCenteredTextShadow("PRESS ANY KEY", cx, py + 598, 32, WHITE);
}

}  // namespace event

int main() {
  AttachParentConsole();

  // Fail loudly and early instead of crashing inside the board parser when
  // the puzzle database is missing.
  if (!Filer::Loaded()) {
    cerr << "Could not load the puzzle database from " << Filer::DatabasePath()
         << endl;
    cerr << "Run scripts/fetch_puzzles.sh (or download the CSV from "
         << "https://database.lichess.org/#puzzles) and try again." << endl;
    return EXIT_FAILURE;
  }

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PuzzledChess");
  InitAudioDevice();
  SetTargetFPS(60);

  if (!event::kEnabled) {
    // The original One Shot / Timed / No Limit menu, left exactly as it was.
    Game classic;
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(BLACK);
      classic.HANDLE_GAME_STATE();
      EndDrawing();
    }
    CloseWindow();
    return 0;
  }

  Texture2D bg = LoadTexture("./assets/background.png");
  SoundMap::Init();
  event::session.Begin();

  Board board;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    event::Change change =
        event::session.Tick(puzzles_solved, score);
    if (change == event::Change::StartRound) {
      score = 0;
      score_str = "0";
      puzzles_solved = 0;
      feedback_text = "";
      board.NewGame();
    }
    if (change == event::Change::FinishRound) {
      event::session.Finish(puzzles_solved, score);
    }

    switch (event::session.screen) {
      case event::Screen::Attract:
        DrawTexture(bg, 0, 0, WHITE);
        event::DrawAttract();
        break;

      case event::Screen::Playing:
        board.setGameState(NO_LIMIT);
        board.Draw();
        board.HandleClick();
        AnimationManager::Update();
        event::DrawHUD();
        break;

      case event::Screen::Results:
        board.Draw();
        AnimationManager::Update();
        event::DrawResults();
        break;
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}