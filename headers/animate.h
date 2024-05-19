#include <iostream>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <unordered_set>
#include <vector>
using namespace std;
#ifndef ANIMATE_H
#define ANIMATE_H

// Make a base class for animations and then make a derived class for each type
// of animations

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

public:
  Animation(Vector2 origin, Vector2 position) {
    this->origin = {origin.x * 100, origin.y * 100};
    this->position = {position.x * 100, position.y * 100};
    // source = {0, 0, (float)sprite->width, (float)sprite->height};
    currentFrame = 0;
    TotalFrames = 30;
    destination = {origin.x * 100, origin.y * 100, 100, 100};
  }

  virtual void Draw() = 0;

  bool Update() {
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

class MovePieceAnim : public Animation {
  Texture2D *sprite;

public:
  MovePieceAnim(Texture2D *sprite, Vector2 origin, Vector2 position)
      : Animation(origin, position) {
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
      : Animation(origin, position) {
    this->text = text;
    // Update Destination to be properly used
    this->origin = {origin.x, origin.y};
    this->position = {position.x, position.y};
    destination = {origin.x, origin.y, 100, 100};
  }
  void Draw() override {
    // cout << "DRAWING THE TEXT " << destination.x << " " << destination.y <<endl;
    DrawText(text.c_str(), destination.x, destination.y, 30, SKYBLUE);
  }
};

class AnimationManager {
  static std::unordered_multiset<Animation *> animations;

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
};

#endif // !ANIMATE_H
