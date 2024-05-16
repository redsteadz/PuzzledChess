#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

enum SoundType {
  Move, Capture
};

class SoundMap {
public:
  static map<SoundType, Sound> soundMap;
  SoundMap() {}
  static void Init() {
    soundMap[Move] = LoadSound("../assets/sound/standard/Move.mp3");
    soundMap[Capture] = LoadSound("../assets/sound/standard/Capture.mp3");
  }
};

#endif
