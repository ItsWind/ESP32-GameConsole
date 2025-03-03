#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

namespace NetImp {
  extern bool DownloadingGame;
  extern AsyncUDP UDP;

  void Init();
  void Update(unsigned long dt);
  void Draw();
  void GetGameDownloadList(void * menu);
  void StartGameDownload(uint8_t index);
  float GetGameDownloadPercentageDone();
}