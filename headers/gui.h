#ifndef GUI_H
#define GUI_H
#include "constant.h"
#include <iostream>
#include <raylib.h>
using namespace std;

// TODO: Make a generic Button class

class Button {
protected:
  bool pressed;
  Rectangle src_button;
  Texture2D *gui;
  Rectangle dst;

public:
  Button(Vector2 pos, Rectangle src_button, Texture2D *gui)
      : src_button(src_button), gui(gui) {
    dst = {pos.x, pos.y, src_button.width * 3, src_button.height * 3};
    pressed = false;
  }
  virtual void Draw() = 0;
  virtual void onClick() = 0;
  virtual void Update() {
    if (CheckCollisionPointRec(GetMousePosition(), dst) &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      pressed = true;
    } else if (pressed) {
      pressed = false;
      onClick();
    }
  }
};

class SmallButton : public Button {
  Rectangle src_icon;

public:
  SmallButton(Vector2 pos, Texture2D *gui)
      : Button(pos, {0, 5 * 16, 16, 16}, gui) {
    src_icon = {8 * 16, 2 * 16, 16, 16};
  }
  void Draw() {
    if (pressed)
      src_button.x = 16;
    else
      src_button.x = 0;
    DrawTexturePro(*gui, src_button, dst, {0, 0}, 0, WHITE);
    DrawTexturePro(
        *gui, src_icon,
        Rectangle{dst.x, dst.y - (pressed ? -2 : 0), dst.width, dst.height},
        {0, 0}, 0, WHITE);
  }
  void onClick() {}
};

// Big button is on the 5th Row 9th col in 16x16 grid

// TODO: Make this a generic button for all types of states
class BigButton : public Button {
  GAME_STATE state;
  GAME_STATE *gameState;
  string text;

public:
  BigButton(Vector2 pos, string text, Texture2D *gui, GAME_STATE state,
            GAME_STATE *gameState)
      : Button(pos, Rectangle{9 * 16, 5 * 16, 16 * 3, 16}, gui), state(state),
        gameState(gameState) {
    this->text = text;
  }
  void Draw() {
    Rectangle drawSrc = src_button;
    if (pressed)
      drawSrc.y = src_button.y + 16;
    DrawTexturePro(*gui, drawSrc, dst, {0, 0}, 0, WHITE);
    DrawText(text.c_str(),
             dst.x + dst.width / 2 - MeasureText(text.c_str(), 20) / 2.0,
             dst.y + 10 + (pressed ? 2 : 0), 20, WHITE);
  }
  void onClick() { *gameState = state; }
};

class Label {
  string *text;
  Rectangle src;
  Texture2D gui;
  Rectangle dst;

public:
  Label(Vector2 pos, string *text) : text(text) {
    gui = LoadTexture("./assets/GUI.png");
    dst = {pos.x, pos.y, 48 * 3, 48};
    src = {3 * 16, 3 * 16, 16 * 3, 16};
  }
  void Draw() {
    DrawTexturePro(gui, src, dst, {0, 0}, 0, WHITE);
    DrawText(text->c_str(),
             dst.x + 48 * 3 / 2 - MeasureText(text->c_str(), 20) / 2,
             dst.y + 10, 20, WHITE);
  }
};

#endif // !GUI_H
