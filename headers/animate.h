#include "constant.h"
#include <iostream>
#include <math.h>
#include <functional>
#include <raylib.h>
#include <raymath.h>
#include <unordered_set>
#include <vector>
using namespace std;
#ifndef ANIMATE_H
#define ANIMATE_H

// Make a base class for animations and then make a derived class for each type
// of animations

enum class AnimationType {
  SquareExpand,
  PieceMove,
  Text
};

class Animation {
protected:
  float duration;
  Rectangle destination;
  Rectangle source;
  Vector2 origin;
  int currentFrame;
  int TotalFrames;
  Vector2 position;
  float dy, dx;
  AnimationType type;
public:
  Animation(Vector2 origin, Vector2 position, AnimationType type) : type(type){
    this->origin = {origin.x * 100, origin.y * 100};
    this->position = {position.x * 100, position.y * 100};
    // source = {0, 0, (float)sprite->width, (float)sprite->height};
    currentFrame = 0;
    TotalFrames = 30;
    destination = {origin.x * 100, origin.y * 100, 100, 100};
  }
  
  AnimationType getType() {
    return type;
  }

  virtual void Draw() = 0;

  virtual bool Update() {
    // Update until destination.x reaches position.x
    // cout << currentFrame << " " << TotalFrames << endl;
    if (currentFrame <= TotalFrames) {
      destination.x =
          Lerp(destination.x, position.x, (float)currentFrame / TotalFrames);
      destination.y =
          Lerp(destination.y, position.y, (float)currentFrame / TotalFrames);
      currentFrame++;
      return true;
    }
    return false;
  }
  virtual ~Animation() {}
};

class ExpandAnimation : public Animation {
  float size;
  // Texture2D *sprite;
  vector<vector<Color>> *vec;
  Color color;
public:
  ExpandAnimation(Vector2 position, double size, vector<vector<Color>> *sB, Color col)
      : Animation(position, position, AnimationType::SquareExpand), size(size), color(col){
    vec = sB;
    this->position = position;
    source = {0, 0, (float)100, (float)100};
    // This is the position to draw
    destination = {position.x * SQUARE_SIZE + gap,
                   position.y * SQUARE_SIZE + gap, SQUARE_SIZE - gap * 2,
                   SQUARE_SIZE - gap * 2};
  }
  // The size expands from the one specified to 1
  bool Update() override {
    // Update until destination.x reaches position.x
    // cout << currentFrame << " " << TotalFrames << endl;
    if (currentFrame <= TotalFrames) {
      size = Lerp(size, 1.0, (float)currentFrame / TotalFrames);
      currentFrame++;
      return true;
    }
    (*vec)[(int)position.y][(int)position.x] = color;
    return false;
  }
  void Draw() override {
    // Use the value of size to set the destination of sprite to be centered but
    // size
    float size_T = 1 - this->size;
    Rectangle destination = {this->destination.x + SQUARE_SIZE * size_T / 2,
                             this->destination.y + SQUARE_SIZE * size_T / 2,
                             this->destination.width * size,
                             this->destination.height * size};
    DrawRectangleRec(destination, color);
  }
};

class MovePieceAnim : public Animation {
  Texture2D *sprite;

public:
  MovePieceAnim(Texture2D *sprite, Vector2 origin, Vector2 position)
      : Animation(origin, position, AnimationType::PieceMove) {
    this->sprite = sprite;
    source = {0, 0, (float)sprite->width, (float)sprite->height};
  }
  void Draw() override {
    DrawTexturePro(*sprite, source, destination, Vector2{0, 0}, 0, WHITE);
  }
};

class TextAnim : public Animation {
  string text;

public:
  TextAnim(string text, Vector2 origin, Vector2 position)
      : Animation(origin, position, AnimationType::Text) {
    this->text = text;
    // Update Destination to be properly used
    this->origin = {origin.x, origin.y};
    this->position = {position.x, position.y};
    destination = {origin.x, origin.y, 100, 100};
  }
  void Draw() override {
    // cout << "DRAWING THE TEXT " << destination.x << " " << destination.y
    // <<endl;
    DrawText(text.c_str(), destination.x, destination.y, 30, SKYBLUE);
  }
};

class AnimationManager {
  // Maybe seperate Game Animations and GUI animations to control them individually ?
  static unordered_multiset<Animation *> animations;
public:
  static void addAnimation(Animation *animation) {
    animations.insert(animation);
  }

  // update all of the animation and remove from list when done with
  static void Update() {
    for (auto it = animations.begin(); it != animations.end();) {
      if (!(*it)->Update()) {
        delete *it;
        it = animations.erase(it);
      } else {
        ++it;
      }
    }
  }

  static void Draw() {
    for (Animation *animation : animations) {
      animation->Draw();
    }
  }
  static void clearBoard() { 
    // Erase all animations of "SquareExpand"
    for (auto it = animations.begin(); it != animations.end();) {
      if ((*it)->getType() == AnimationType::SquareExpand) {
        it = animations.erase(it);
      } else {
        ++it;
      }
    }
  }
};

#endif // !ANIMATE_H
