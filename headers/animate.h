#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;
#ifndef ANIMATE_H
#define ANIMATE_H

class Animation {
  Texture2D *sprite;
  float duration;
  Rectangle destination;
  Rectangle source;
  Vector2 origin;
  int currentFrame;
  int TotalFrames;
  Vector2 position;
  float dy, dx;
public:
  Animation(Texture2D *sprite, Vector2 origin, Vector2 position): sprite(sprite){
    this -> origin = {origin.x*100 + 12, origin.y*100 + 12};
    this -> position = {position.x*100 + 12, position.y*100 + 12};
    source = {0, 0, (float)sprite->width, (float)sprite->height};
    currentFrame = 0;
    TotalFrames = 30;
    destination = {origin.x*100 + 12, origin.y*100 + 12, (float)sprite->width, (float)sprite->height};
  }
  
  void Draw() {
    // cout << destination.x << " " << destination.y << endl;
    DrawTexturePro(*sprite, source, destination, Vector2{0,0}, 0, WHITE);  
  }

  bool Update(){
    // Update until destination.x reaches position.x
    // cout << currentFrame << " " << TotalFrames << endl;
    if (currentFrame <= TotalFrames){
      destination.x = Lerp(destination.x, position.x, (float)currentFrame/TotalFrames);
      destination.y = Lerp(destination.y, position.y, (float)currentFrame/TotalFrames);
      currentFrame++;
      return true;
    }
    return false;
  }
};

class AnimationManager{
  static vector<Animation> animations;
  public:
    static void addAnimation(Animation animation){
      animations.push_back(animation);
    }
    // update all of the animation and remove from list when done with 
    static void Update(){
      int i = 0;
      for (Animation &animation : animations){
        if (!animation.Update()){
          animations.erase(animations.begin() + i);
        }
        i++;
      }
    }
    
    static void Draw(){
      for (Animation animation : animations){
        animation.Draw();
      }
    }
};


#endif // !ANIMATE_H
