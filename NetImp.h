#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

namespace NetImp {
  extern const char * WifiSsid;
  extern const char * WifiPass;
  extern AsyncUDP UDP;

  void Init();
  void CheckConnection();
}