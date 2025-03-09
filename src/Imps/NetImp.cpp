#include "src/Imps/NetImp.h"
#include "secrets.h"
#include "src/Imps/MenuImp.h"
#include "src/Imps/TFTImp.h"
#include "src/Imps/FileImp.h"

static const char * WifiSsid = SECRET_WIFI_SSID;
static const char * WifiPass = SECRET_WIFI_PASS;

static unsigned long downloadingFilePulse = 0;
static char * fileDirNameDownloading = nullptr;
static uint16_t currentChunkNum = 0;

static uint16_t currentFileCount = 0;
static uint16_t doneFileCount = 0;

static uint32_t currentByteCount = 0;
static uint32_t doneByteCount = 0;

static MenuImp::InstallMenu * installMenuDump = nullptr;

static void beginUDPMasterConnection() {
  if (NetImp::UDP.connect(IPAddress(54,39,21,229), 33733)) {
    NetImp::UDP.onPacket([](AsyncUDPPacket packet) {
      // Receive packet
      const uint8_t * bytes = packet.data();
      NetImp::ProcessPacket(bytes, packet.length(), false);
    });
  }
}

static void checkConnection() {
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

static void checkDownloadPulse(unsigned long dt) {
  if (fileDirNameDownloading == nullptr) {
    return;
  }

  NetImp::DownloadingGame = true;
  downloadingFilePulse += dt;
  if (downloadingFilePulse >= 2000000) {
    downloadingFilePulse = 0;
    delete[] fileDirNameDownloading;
    fileDirNameDownloading = nullptr;
    NetImp::DownloadingGame = false;
  }
}

namespace NetImp {
  bool DownloadingGame = false;

  AsyncUDP UDP;

  void Init() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WifiSsid, WifiPass);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      Serial.println("Beginning UDP connection.");
      beginUDPMasterConnection();
    }
  }

  void Update(unsigned long dt) {
    checkConnection();
    checkDownloadPulse(dt);
  }

  void Draw() {
    int16_t screenWidthHalf = TFTImp::Screen.width() / 2;
    int16_t screenHeightHalf = TFTImp::Screen.height() / 2;

    TFTImp::DrawCenteredText("Downloading game...");
    TFTImp::DrawCenteredText(screenWidthHalf, screenHeightHalf + 12, fileDirNameDownloading);
    String fileBytesOutOfBytesStr = "( " + String(doneByteCount) + " / " + String(currentByteCount) + " )";
    TFTImp::DrawCenteredText(screenWidthHalf, screenHeightHalf + 24, fileBytesOutOfBytesStr.c_str());
    TFTImp::FrameSprite.fillRect(30, 100, 100, 10, TFT_RED);
    TFTImp::FrameSprite.fillRect(30, 100, (int32_t)(GetGameDownloadPercentageDone() * 100.0), 10, TFT_GREEN);
  }

  void GetGameDownloadList(void * menu) {
    installMenuDump = (MenuImp::InstallMenu *)menu;

    uint8_t bytes[] = {255};
    UDP.write(bytes, 1);
  }

  void CancelGameDownloadList() {
    installMenuDump = nullptr;
  }

  void StartGameDownload(uint8_t index) {
    uint8_t bytes[] = {1, index};
    UDP.write(bytes, 2);
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

  void ProcessPacket(const uint8_t * bytes, size_t len, bool fromSerial) {
    switch (bytes[0]) {
      // 0 = getting game directory for clearing, prefixes {0, highFileCountByte, lowFileCountByte}
      case 0:
      {
        currentFileCount = (bytes[1] << 8) | bytes[2];
        doneFileCount = 0;
        Serial.println(currentFileCount);

        char gameDirectoryName[len - 2];
        memcpy(&gameDirectoryName, &bytes[3], len - 3);
        gameDirectoryName[len - 3] = '\0';

        String fullPath = "/games/" + String(gameDirectoryName);
        Serial.println(fullPath.c_str());
        FileImp::NukeDirectory(fullPath.c_str());

        uint8_t bytesBackClearedDir[] = {2};
        if (fromSerial) {
          // Send bytes through serial
        }
        else {
          NetImp::UDP.write(bytesBackClearedDir, 1);
        }
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

        fileDirNameDownloading = new char[len - 4];
        memcpy(fileDirNameDownloading, &bytes[5], len - 5);
        fileDirNameDownloading[len - 5] = '\0';
        currentChunkNum = 0;

        Serial.print("Setting current downloading file to ");
        Serial.println(fileDirNameDownloading);

        uint8_t bytesBackFileName[] = {3};
        if (fromSerial) {
          // Send bytes through serial
        }
        else {
          NetImp::UDP.write(bytesBackFileName, 1);
        }
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
          if (fromSerial) {
            // Send bytes through serial
          }
          else {
            NetImp::UDP.write(bytesBackChunk, 3);
          }
          return;
        }
        else if (chunkNum > currentChunkNum) {
          Serial.println("FATAL DOWNLOAD ERROR. CHUNK NUM GREATER THAN CURRENT. THIS SHOULD NOT HAPPEN AT ALL.");
          return;
        }
        currentChunkNum++;
        
        size_t packetByteCount = len - 3;
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
          if (fromSerial) {
            // Send bytes through serial
          }
          else {
            NetImp::UDP.write(bytesBackChunk, 3);
          }
        }
        return;
      }
      case 255:
      {
        if (installMenuDump == nullptr) {
          return;
        }

        uint8_t gameDownloadListCount = 0;
        char ** gameDownloadList = new char *[256];

        char dirList[len];
        memcpy(dirList, &bytes[1], len - 1);
        dirList[len - 1] = '\0';

        String dirListStr = String(dirList);

        int indexOfSep = dirListStr.indexOf('/');
        while (indexOfSep != -1) {
          String thisDirName = dirListStr.substring(0, indexOfSep);
          char * dirNameForList = new char[thisDirName.length() + 1];
          memcpy(dirNameForList, thisDirName.c_str(), thisDirName.length());
          dirNameForList[thisDirName.length()] = '\0';

          gameDownloadList[gameDownloadListCount] = dirNameForList;
          gameDownloadListCount++;

          if (indexOfSep != dirListStr.length() - 1) {
            dirListStr = dirListStr.substring(indexOfSep + 1);
            indexOfSep = dirListStr.indexOf('/');
          }
          else {
            indexOfSep = -1;
          }
        }
        
        // Check if install menu is still valid
        if (installMenuDump != nullptr) {
          installMenuDump->DumpDownloadList(gameDownloadList, gameDownloadListCount);
          installMenuDump = nullptr;
        }
        else {
          for (uint8_t i = 0; i < gameDownloadListCount; i++) {
            delete[] gameDownloadList[i];
          }
          delete[] gameDownloadList;
        }
      }
    }
  }
}