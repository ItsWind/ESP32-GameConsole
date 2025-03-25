#pragma once

#include <Arduino.h>
#include <Audio.h>

namespace SoundImp {
  void Init();
  void Update();
  void PlaySoundFromFileSystem(const char * filePath);
  void PlayVoiceFromWiFi(const char * say);
}