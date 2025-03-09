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
  void CancelGameDownloadList();
  void StartGameDownload(uint8_t index);
  float GetGameDownloadPercentageDone();
  void ProcessPacket(const uint8_t * bytes, size_t len, bool fromSerial);
}