#include "map"
#include "raylib.h"
#include <ios>
#include <iostream>
#include <string>
using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define SQUARE_SIZE 100

class Board {
  string board[8][8];
  map<string, Texture2D> texs;

public:
  Board() {
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
        board[i][j] = "";
    // Load textures for each pieces
    string pieces[16] = {"wP", "wN", "wB", "wR", "wQ", "wK",
                         "bP", "bN", "bB", "bR", "bQ", "bK"};
    for (string name : pieces) {
      string texName = "../assets/" + name + ".png";
      texs[name] = LoadTexture(texName.c_str());
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
      if (c <= '9' && c >= '1'){
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
        Color squareColor = ((x + y) % 2 == 0) ? GRAY : WHITE;
        DrawRectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE,
                      SQUARE_SIZE, squareColor);
        // Draw the piece
        if (board[y][x] == "")
          continue;

        DrawTexture(texs[board[y][x]], x * SQUARE_SIZE + 12, y * SQUARE_SIZE + 12, WHITE);
      }
    }
  }
};

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Grid");

  SetTargetFPS(60);
  Board board;
  board.setBoard("r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1");

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();
    ClearBackground(WHITE);
    board.Draw();
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
