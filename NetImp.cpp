#include "NetImp.h"
#include "secrets.h"
#include "TFTImp.h"
#include "FileImp.h"

static void beginUDPMasterConnection() {
  if (NetImp::UDP.connect(IPAddress(54,39,21,229), 33733)) {
    NetImp::UDP.onPacket([](AsyncUDPPacket packet) {
      // Receive packet
      const uint8_t * bytes = packet.data();
      switch (bytes[0]) {
        // 0 = getting dir/file.extension, prefixes {0}
        case 0:

          break;
        // 1 = getting chunk of above dirFilePath, prefixes {1, chunkNum}
        case 1:

          break;
      }
    });
  }
}

namespace NetImp {
  const char * WifiSsid = SECRET_WIFI_SSID;
  const char * WifiPass = SECRET_WIFI_PASS;

  AsyncUDP UDP;

  void Init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WifiSsid, WifiPass);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println("Beginning UDP connection.");
      beginUDPMasterConnection();
    }
  }

  void CheckConnection() {
    if (WiFi.status() == WL_CONNECTED) {
      return;
    }
    Serial.println("Wifi lost. Attempting reconnect.");
    
    WiFi.begin(WifiSsid, WifiPass);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println("Beginning UDP connection.");
      beginUDPMasterConnection();
    }
  }
}