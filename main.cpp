#include "cmath"
#include "map"
#include "raylib.h"
#include <bits/stdc++.h>
#include <ios>
#include <iostream>
#include <string>
using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define SQUARE_SIZE 100

class Piece {
  string name;

public:
  Piece(string name) { this->name = name; }
  void virtual ViewMoves() = 0;
};

class HandlePiece {
public:
  static void Pawn(string name, Vector2 pos, Color selectedBoard[8][8]) {
    char enemy = name[0] == 'w' ? 'b' : 'w';
    bool homeRow = ((name[0] == 'w') ? 7 - pos.y : pos.y) == 1;
    int mag = homeRow ? 2 : 1;
    // Direction
    int dir = name[0] == 'w' ? 1 : -1;
    // If dir + color the board from pos.y += dir for mag
    for (int i = 1; i <= mag; i++)
      selectedBoard[(int)pos.y - i * dir][(int)pos.x] = BLUE;
  }

  static void Bishop(string name, Vector2 pos, Color selectedBoard[8][8]) {
    // Highlight all the diagonals if in bounds
    // Top Right
    for (int i = 1; i < 8; i++) {
      if (pos.x + i < 8 && pos.y - i > -1) {
        selectedBoard[(int)pos.y - i][(int)pos.x + i] = BLUE;
      }
    }
    // Top Left
    for (int i = 1; i < 8; i++) {
      if (pos.x - i > -1 && pos.y - i > -1) {
        selectedBoard[(int)pos.y - i][(int)pos.x - i] = BLUE;
      }
    }
    // Bottom Right
    for (int i = 1; i < 8; i++) {
      if (pos.x + i < 8 && pos.y + i < 8) {
        selectedBoard[(int)pos.y + i][(int)pos.x + i] = BLUE;
      }
    }
    // Bottom Left
    for (int i = 1; i < 8; i++) {
      if (pos.x - i > -1 && pos.y + i < 8) {
        selectedBoard[(int)pos.y + i][(int)pos.x - i] = BLUE;
      }
    }
  }

  static void Rook(string name, Vector2 pos, Color selectedBoard[8][8]) {
    // Highlight all the rows if in bounds
    for (int i = 1; i < 8; i++) {
      selectedBoard[i][(int)pos.x] = BLUE;
    }
    // Highlight all the columns if in bounds
    for (int i = 1; i < 8; i++) {
      selectedBoard[(int)pos.y][i] = BLUE;
    }
  }

  void static Knight(string name, Vector2 pos, Color selectedBoard[8][8]) {
    // Highlight all the rows if in bounds

    int delrow[8] = {+2, -1, +1, -2, +2, +1, -1, -2};
    int delcol[8] = {+1, +2, +2, +1, -1, -2, -2, -1};
    for (int i = 0; i < 8; i++) {
      Vector2 newP = {pos.x + delrow[i], pos.y + delcol[i]};
      if (newP.x < 8 && newP.y < 8 && newP.x > -1 && newP.y > -1)
        selectedBoard[(int)newP.y][(int)newP.x] = BLUE;
    }
  }

  static void King(string name, Vector2 pos, Color selectedBoard[8][8]) {
    int dx[] = {0, 1, -1};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int x = pos.x + dx[i];
        int y = pos.y + dx[j];
        if (x < 8 && y < 8 && x > -1 && y > -1)
          selectedBoard[y][x] = BLUE;
      }
    }
  }

  static void Queen(string name, Vector2 pos, Color selectedBoard[8][8]) {
    Bishop(name, pos, selectedBoard);
    Rook(name, pos, selectedBoard);
  }

  static void PieceEval(string name, Vector2 pos, Color selectedBoard[8][8]) {
    if (name.length() < 2)
      return;
    char piece = name[1];
    switch (piece) {
    case 'P':
      Pawn(name, pos, selectedBoard);
      break;
    case 'N':
      Knight(name, pos, selectedBoard);
      break;
    case 'B':
      Bishop(name, pos, selectedBoard);
      break;
    case 'R':
      Rook(name, pos, selectedBoard);
      break;
    case 'Q':
      Queen(name, pos, selectedBoard);
      break;
    case 'K':
      King(name, pos, selectedBoard);
      break;
    default:
      cout << "Piece not found";
      break;
    }
  }
};

class Board {
  string board[8][8];
  Color colorBoard[8][8];
  Color selectedBoard[8][8];
  map<string, Texture2D> texs;
  bool toggleSelect = 0;
  Vector2 start = {-1, -1}, end = {-1, -1};
  vector<pair<Vector2, Vector2>> moves;
  int moveCount = 0;
  Vector2 prevMousePos;
  int saveMove = 0;
  int gap = 5;

public:
  Board() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? GRAY : WHITE;
        colorBoard[i][j] = squareColor;
        board[i][j] = "";
        selectedBoard[i][j] = WHITE;
      }
    // Load textures for each pieces
    string pieces[] = {"wP", "wN", "wB", "wR", "wQ", "wK",
                       "bP", "bN", "bB", "bR", "bQ", "bK"};
    for (string name : pieces) {
      string texName = "../assets/" + name + ".png";
      texs[name] = LoadTexture(texName.c_str());
    }
  }
  void ParseMoves(string moveSet) {
    // Break the string into pieces
    istringstream iss(moveSet);
    string move;
    vector<string> movesS;
    while (getline(iss, move, ' ')) {
      movesS.push_back(move);
    }
    for (string m : movesS) {
      Vector2 start = {((float)m[0] - 'a'), 7 - ((float)m[1] - '1')};
      Vector2 end = {(float)m[2] - 'a', 7 - ((float)m[3] - '1')};
      pair<Vector2, Vector2> p = {start, end};
      moves.push_back(p);
    }
  }
  void setBoard(string FEN) {
    int i = 0, j = 0;
    for (char c : FEN) {
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
  }
  void Draw() {
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        // Draw the square
        DrawRectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE,
                      SQUARE_SIZE, colorBoard[y][x]);
        if (!CompareColor(selectedBoard[y][x], WHITE))
          DrawRectangle(x * SQUARE_SIZE + gap, y * SQUARE_SIZE + gap,
                        SQUARE_SIZE - gap * 2, SQUARE_SIZE - gap * 2,
                        selectedBoard[y][x]);
        // Draw the piece
        if (board[y][x] == "")
          continue;

        DrawTexture(texs[board[y][x]], x * SQUARE_SIZE + 12,
                    y * SQUARE_SIZE + 12, WHITE);
      }
    }
  }
  void HandleClick() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 pos = MousePos();
      cout << pos.y << " " << pos.x << endl;
      ResetBoardColor();
      HandlePiece::PieceEval(board[(int)pos.y][(int)pos.x], pos, selectedBoard);
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
    moveCount++;
  }

private:
  void ResetBoardColor() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? GRAY : WHITE;
        colorBoard[i][j] = squareColor;
        selectedBoard[i][j] = WHITE;
      }
    prevMousePos = {-1, -1};
    start = {-1, -1};
    end = {-1, -1};
  }
  Vector2 MousePos() {
    Vector2 mouse = GetMousePosition();
    Vector2 pos = {floor(mouse.x / SQUARE_SIZE), floor(mouse.y / SQUARE_SIZE)};
    return pos;
  }
  bool VerifyMove() {}
  bool CompareColor(Color a, Color b) {
    Vector3 a_V = ColorToHSV(a);
    Vector3 b_V = ColorToHSV(b);
    return a_V.x == b_V.x && a_V.y == b_V.y && a_V.z == b_V.z;
  }
};

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Grid");

  SetTargetFPS(60);
  Board board;
  board.setBoard("q3k1nr/1pp1nQpp/3p4/1P2p3/4P3/B1PP1b2/B5PP/5K2");
  board.ParseMoves("e8d7 a2e6 d7d8 f7f8");
  board.PrintMoves();
  board.EnemyMove();
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground(WHITE);
    board.Draw();
    board.HandleClick();
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
