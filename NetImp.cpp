#include "NetImp.h"
#include "secrets.h"
#include "TFTImp.h"
#include "FileImp.h"

static char * fileDirNameDownloading = nullptr;
static uint16_t currentChunkNum = 0;

static void beginUDPMasterConnection() {
  if (NetImp::UDP.connect(IPAddress(54,39,21,229), 33733)) {
    NetImp::UDP.onPacket([](AsyncUDPPacket packet) {
      // Receive packet
      const uint8_t * bytes = packet.data();
      switch (bytes[0]) {
        // 0 = getting game directory for clearing, prefixes {0}
        case 0:
        {
          char gameDirectoryName[packet.length()];
          memcpy(&gameDirectoryName, &bytes[1], packet.length() - 1);
          gameDirectoryName[packet.length() - 1] = '\0';

          String fullPath = "/games/" + String(gameDirectoryName);
          FileImp::NukeDirectory(fullPath.c_str());

          uint8_t bytesBackClearedDir[] = {2};
          NetImp::UDP.write(bytesBackClearedDir, 1);
          return;
        }
        // 1 = getting dir/file.extension, prefixes {1}
        case 1:
        {
          if (fileDirNameDownloading != nullptr) {
            delete[] fileDirNameDownloading;
          }

          fileDirNameDownloading = new char[packet.length()];
          memcpy(fileDirNameDownloading, &bytes[1], packet.length() - 1);
          fileDirNameDownloading[packet.length() - 1] = '\0';
          currentChunkNum = 0;

          Serial.print("Setting current downloading file to ");
          Serial.println(fileDirNameDownloading);
          uint8_t bytesBackFileName[] = {3};
          NetImp::UDP.write(bytesBackFileName, 1);
          return;
        }
        // 2 = getting chunk of above dirFilePath, prefixes {2, chunkNumByte1, chunkNumByte2}
        case 2:
        {
          uint16_t chunkNum = (bytes[1] << 8) | bytes[2];
          Serial.println(bytes[1]);
          Serial.println(bytes[2]);
          Serial.println(chunkNum);

          if (chunkNum != currentChunkNum) {
            return;
          }
          currentChunkNum++;

          if (FileImp::AppendBytesToGameFile(fileDirNameDownloading, &bytes[3], packet.length() - 3)) {
            Serial.print("Appended bytes to ");
            Serial.println(fileDirNameDownloading);
            uint8_t bytesBackChunk[] = {4, bytes[1], bytes[2]};
            NetImp::UDP.write(bytesBackChunk, 3);
          }
          return;
        }
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