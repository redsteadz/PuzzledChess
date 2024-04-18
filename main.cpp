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

class Board {
  string board[8][8];
  Color colorBoard[8][8];
  map<string, Texture2D> texs;
  int ch = 0;
  Vector2 start = {-1, -1}, end = {-1, -1};
  vector<pair<Vector2, Vector2>> moves;
  int moveCount = 0;
  Vector2 prevMousePos;

public:
  Board() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? GRAY : WHITE;
        colorBoard[i][j] = squareColor;
        board[i][j] = "";
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
        // Draw the piece
        if (board[y][x] == "")
          continue;

        DrawTexture(texs[board[y][x]], x * SQUARE_SIZE + 12,
                    y * SQUARE_SIZE + 12, WHITE);
      }
    }
  }
  void HandleClick() {

    Vector2 pos = MousePos();
    if (pos.x < 0 || pos.x > 7 || pos.y < 0 || pos.y > 7)
      return;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      // Get the mouse position
      // Draw the hover effect
      cout << pos.y << " " << pos.x << endl;
      colorBoard[(int)pos.y][(int)pos.x] = BLUE;
      if (++ch > 1) {
        end = pos;
        // VerifyMove();
        ResetBoardColor();
        ch = 0;
      }
      if (ch == 1)
        start = pos;

    }
    if (ch == 1) {
      // Make a hover effect
      if (prevMousePos.x >= 0 && prevMousePos.y >= 0 &&
          (prevMousePos.x != start.x || prevMousePos.y != start.y)) {
        // Set it default color

        colorBoard[(int)prevMousePos.y][(int)prevMousePos.x] =
            ((int)(prevMousePos.x + prevMousePos.y) % 2 == 0) ? GRAY : WHITE;
      }
      prevMousePos = pos;
      colorBoard[(int)pos.y][(int)pos.x] = ORANGE;
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

private:
  void ResetBoardColor() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) {
        Color squareColor = ((i + j) % 2 == 0) ? GRAY : WHITE;
        colorBoard[i][j] = squareColor;
      }
    prevMousePos = {-1, -1};
  }
  Vector2 MousePos() {
    Vector2 mouse = GetMousePosition();
    Vector2 pos = {floor(mouse.x / SQUARE_SIZE), floor(mouse.y / SQUARE_SIZE)};
    return pos;
  }
  bool VerifyMove() {
    if (moveCount < moves.size() && start.x == moves[moveCount].first.x &&
        start.y == moves[moveCount].first.y &&
        end.x == moves[moveCount].second.x &&
        end.y == moves[moveCount].second.y) {
      moveCount++;
      if (moveCount == moves.size())
        cout << "WON" << endl;
      return true;
    } else {
      moveCount = 0;
      start = {-1, -1};
      end = {-1, -1};
      return false;
    }
  }
};

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Grid");

  SetTargetFPS(60);
  Board board;
  board.setBoard("q3k1nr/1pp1nQpp/3p4/1P2p3/4P3/B1PP1b2/B5PP/5K2");
  board.ParseMoves("e8d7 a2e6 d7d8 f7f8");
  board.PrintMoves();
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
