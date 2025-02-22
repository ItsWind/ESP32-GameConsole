#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

namespace NetImp {
  extern bool DownloadingGame;
  extern const char * WifiSsid;
  extern const char * WifiPass;
  extern AsyncUDP UDP;

  void Init();
  void CheckConnection();
  void StartGameDownload(uint8_t index);
  void CheckGameDownloadPulse(unsigned long dt);
  float GetGameDownloadPercentageDone();
}