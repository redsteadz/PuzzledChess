#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

enum SoundType {
  Move, Capture, SCORE
};

class SoundMap {
public:
  static map<SoundType, Sound> soundMap;
  SoundMap() {}
  static void Init() {
    soundMap[Move] = LoadSound("./assets/sound/standard/Move.mp3");
    soundMap[Capture] = LoadSound("./assets/sound/standard/Capture.mp3");
    soundMap[SCORE] = LoadSound("./assets/sound/base/pointUp.wav");
  }
};

#endif
