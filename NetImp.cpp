#include "NetImp.h"
#include "secrets.h"
#include "TFTImp.h"
#include "FileImp.h"

static unsigned long downloadingFilePulse = 0;
static char * fileDirNameDownloading = nullptr;
static uint16_t currentChunkNum = 0;

static uint16_t currentFileCount = 0;
static uint16_t doneFileCount = 0;

static uint32_t currentByteCount = 0;
static uint32_t doneByteCount = 0;

static void beginUDPMasterConnection() {
  if (NetImp::UDP.connect(IPAddress(54,39,21,229), 33733)) {
    NetImp::UDP.onPacket([](AsyncUDPPacket packet) {
      // Receive packet
      const uint8_t * bytes = packet.data();
      switch (bytes[0]) {
        // 0 = getting game directory for clearing, prefixes {0, highFileCountByte, lowFileCountByte}
        case 0:
        {
          currentFileCount = (bytes[1] << 8) | bytes[2];
          doneFileCount = 0;
          Serial.println(currentFileCount);

          char gameDirectoryName[packet.length() - 2];
          memcpy(&gameDirectoryName, &bytes[3], packet.length() - 3);
          gameDirectoryName[packet.length() - 3] = '\0';

          String fullPath = "/games/" + String(gameDirectoryName);
          Serial.println(fullPath.c_str());
          FileImp::NukeDirectory(fullPath.c_str());

          uint8_t bytesBackClearedDir[] = {2};
          NetImp::UDP.write(bytesBackClearedDir, 1);
          return;
        }
        // 1 = getting dir/file.extension, prefixes {1, b1, b2, b3, b4}
        case 1:
        {
          currentByteCount = (bytes[1] << 24) | (bytes[2] << 16) | (bytes[3] << 8) | bytes[4];
          doneByteCount = 0;
          Serial.println(currentByteCount);

          downloadingFilePulse = 0;

          if (fileDirNameDownloading != nullptr) {
            delete[] fileDirNameDownloading;
          }

          fileDirNameDownloading = new char[packet.length() - 4];
          memcpy(fileDirNameDownloading, &bytes[5], packet.length() - 5);
          fileDirNameDownloading[packet.length() - 5] = '\0';
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
          if (fileDirNameDownloading == nullptr) {
            return;
          }
          downloadingFilePulse = 0;
          
          uint16_t chunkNum = (bytes[1] << 8) | bytes[2];
          Serial.println(bytes[1]);
          Serial.println(bytes[2]);
          Serial.println(chunkNum);

          if (chunkNum < currentChunkNum) {
            Serial.println("Caught resend current chunk num error");
            uint8_t bytesBackChunk[] = {4, bytes[1], bytes[2]};
            NetImp::UDP.write(bytesBackChunk, 3);
            return;
          }
          else if (chunkNum > currentChunkNum) {
            Serial.println("FATAL DOWNLOAD ERROR. CHUNK NUM GREATER THAN CURRENT. THIS SHOULD NOT HAPPEN AT ALL.");
            return;
          }
          currentChunkNum++;
          
          size_t packetByteCount = packet.length() - 3;
          doneByteCount += packetByteCount;
          if (doneByteCount >= currentByteCount) {
            doneFileCount++;
            Serial.print("DONE FILE COUNT: ");
            Serial.println(doneFileCount);
            Serial.println(NetImp::GetGameDownloadPercentageDone());
          }

          if (FileImp::AppendBytesToGameFile(fileDirNameDownloading, &bytes[3], packetByteCount)) {
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
  bool DownloadingGame = false;
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

  void StartGameDownload(uint8_t index) {
    uint8_t bytes[] = {1, index};
    UDP.write(bytes, 2);
  }

  void CheckGameDownloadPulse(unsigned long dt) {
    if (fileDirNameDownloading == nullptr) {
      return;
    }

    DownloadingGame = true;
    downloadingFilePulse += dt;
    if (downloadingFilePulse >= 2000000) {
      downloadingFilePulse = 0;
      fileDirNameDownloading = nullptr;
      DownloadingGame = false;
    }
  }

  float GetGameDownloadPercentageDone() {
    if (!DownloadingGame) {
      return 0;
    }
    else if (doneFileCount >= currentFileCount) {
      return 1;
    }

    float baseFilePercentShare = 1.0 / (float)currentFileCount;
    
    float currentMinPercent = baseFilePercentShare * doneFileCount;
    float addToMin = 0;
    if (doneByteCount < currentByteCount) {
      addToMin = baseFilePercentShare * ((float)doneByteCount / (float)currentByteCount);
    }

    return currentMinPercent + addToMin;
  }
}