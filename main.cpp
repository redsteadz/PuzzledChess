#include "cmath"
#include "headers/animate.h"
#include "headers/constant.h"
#include "headers/database.h"
#include "headers/gui.h"
#include "headers/sound.h"
#include "map"
#include "raylib.h"
#include <bits/stdc++.h>
#include <ios>
#include <iostream>
#include <string>
#include <utility>
using namespace std;

string board[8][8];
Color colorBoard[8][8];
vector<vector<Color>> selectedBoard(8, vector<Color>(8));
int score = 0;
string score_str = "0";

string UpdateTime(int time_f) {
  int hours = time_f / 3600;
  int minutes = (time_f % 3600) / 60;
  int seconds = time_f % 60;

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0')
      << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2)
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
          if (board[valy][valx][0] == enemy){
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

    // Highlight all the diagonals if in bounds
    // Top Right
    for (int i = 1; i < 8; i++) {
      Vector2 newPos = {pos.x + i, pos.y - i};
      if (inBounds(Vector2{pos.x + i, pos.y - i})) {
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
        if (board[(int)newP.y][(int)newP.x] == ""){
          // selectedBoard[(int)newP.y][(int)newP.x] = BLUE;
          ExpandAnimation *anim =
              new ExpandAnimation(newP, 0.2, &selectedBoard, BLUE);
        AnimationManager::addAnimation(anim);
        }
        if (board[(int)newP.y][(int)newP.x][0] == enemy){
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
          if (board[y][x][0] == enemy){
            // selectedBoard[y][x] = RED;
            ExpandAnimation *anim =
                new ExpandAnimation({(float)x, (float)y}, 0.2, &selectedBoard, RED);
            AnimationManager::addAnimation(anim);
          }
          if (board[y][x] == ""){
            // selectedBoard[y][x] = BLUE;
            ExpandAnimation *anim =
                new ExpandAnimation({(float)x, (float)y}, 0.2, &selectedBoard, BLUE);
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
          if (moveCount < moves.size())
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
    if (moveCount < moves.size())
      EnemyMove();
    else {
      UpdateScore();
      NewGame();
    }
  }
  void UpdateScore() {
    double detalTime = GetTime() - timeStarted;
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
    TextAnim *scoreAnim =
        new TextAnim(score_str, Vector2{650, 800}, Vector2{650, 700});
    AnimationManager::addAnimation(scoreAnim);
  }
  void NewGame() {
    pair<string, string> Board_Moves = Filer::ParseLine();
    cout << Board_Moves.first << endl;
    cout << Board_Moves.second << endl;
    setBoard(Board_Moves.first);
    ParseMoves(Board_Moves.second);
    cout << "THE MOVES -------------->" << endl;
    PrintMoves();
    EnemyMove();
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

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Grid");
  InitAudioDevice();
  SetTargetFPS(60);
  Board b;
  b.setBoard("r3r1k1/p4ppp/2p2n2/1p6/3P1qb1/2NQR3/PPB2PP1/R1B3K1 w - - 5 18");
  b.ParseMoves("e3g3 e8e1 g1h2 e1c1 a1c1 f4h6 h2g1 h6c1");
  b.PrintMoves();
  b.EnemyMove();
  SoundMap::Init();
  string time = "00:00:00";
  // Make a function that updates the string time
  Label Time(Vector2{400 - 48 * 3 / 2, 800}, &time);
  Label score(Vector2{650, 800}, &score_str);
  SmallButton Button(Vector2{0, 800});
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground(BLACK);
    time = UpdateTime((int)GetTime());
    if (IsKeyPressed(KEY_SPACE)) {
      // cout << Filer::ReadRandomLineFromFile() << endl;
      pair<string, string> Board_Moves = Filer::ParseLine();
      cout << Board_Moves.first << endl;
      cout << Board_Moves.second << endl;
      b.setBoard(Board_Moves.first);
      b.ParseMoves(Board_Moves.second);
      cout << "THE MOVES -------------->" << endl;
      b.PrintMoves();
      b.EnemyMove();
    }
    if (IsKeyPressed(KEY_RIGHT))
      b.Cheat();
    b.Draw();
    Button.Update();
    Time.Draw();
    score.Draw();
    Button.Draw();
    // AnimationManager::Draw();
    AnimationManager::Update();
    b.HandleClick();
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
