#ifndef GUI_H
#define GUI_H
#include <iostream>
#include <raylib.h>
using namespace std;

class SmallButton {
  bool pressed;
  Rectangle src_button;
  Rectangle src_icon;
  Texture2D gui;
  Rectangle dst;

public:
  SmallButton(Vector2 pos) {
    gui = LoadTexture("../assets/GUI.png");
    dst = {pos.x, pos.y, 48, 48};
    pressed = false;
    src_button = {0, 5 * 16, 16, 16};
    src_icon = {8 * 16, 2 * 16, 16, 16};
  }
  void Draw() {
    if (pressed)
      src_button.x = 16;
    else
      src_button.x = 0;
    DrawTexturePro(gui, src_button, dst, {0, 0}, 0, WHITE);
    DrawTexturePro(gui, src_icon,
                   Rectangle{dst.x, dst.y - (pressed ? -2:0), dst.width, dst.height}, {0, 0}, 0,
                   WHITE);
  }
  void Update() {
    if (CheckCollisionPointRec(GetMousePosition(), dst) &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      pressed = true;
    } else
      pressed = false;
  }
};

class Label{
  string* text;
  Rectangle src;
  Texture2D gui;
  Rectangle dst;

public:
  Label(Vector2 pos, string *text): text(text) {
    gui = LoadTexture("../assets/GUI.png");
    dst = {pos.x, pos.y, 48*3, 48};
    src = {3*16, 3*16, 16*3, 16};
  }
  void Draw() {
    DrawTexturePro(gui, src, dst, {0, 0}, 0, WHITE);
    DrawText(text->c_str(), dst.x + 48/2 + 10, dst.y + 10, 20, WHITE);
  }
};

#endif // !GUI_H
