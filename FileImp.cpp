#include "FileImp.h"
#include "FS.h"
#include "LittleFS.h"

namespace FileImp {
  void Init() {
    if (!LittleFS.begin()) {
      Serial.println("LittleFS begin failed, trying format.");
      if (!LittleFS.format()) {
        Serial.println("LittleFS format failed");
        while (true) {}
      }
      if (!LittleFS.begin()) {
        Serial.println("LittleFS begin failed");
        while (true) {}
      }
    }
  }

  void CreateDirectory(const char * dirName) {
    if (LittleFS.mkdir(dirName)) {
      Serial.println("Directory created");
    }
    else {
      Serial.println("Directory failed");
    }
  }

  void RemoveDirectory(const char * dirName) {
    if (LittleFS.rmdir(dirName)) {
      Serial.println("Directory removed");
    }
    else {
      Serial.println("Directory failed to remove");
    }
  }

  void RemoveFile(const char * dirAndFileName) {
    String fullPath = "/games/" + String(dirAndFileName);

    LittleFS.remove(fullPath.c_str());
  }

  bool AppendBytesToGameFile(const char * dirAndFileName, const uint8_t * bytes, uint length) {
    String fullPath = "/games/" + String(dirAndFileName);
    bool wrote = false;

    File file = LittleFS.open(fullPath.c_str(), FILE_APPEND, true);
    if (file) {
      wrote = true;
      file.write(bytes, length);
    }
    file.close();
    return wrote;
  }

  char * GetGameMainData(const char * gameDirName) {
    String fullPath = "/games/" + String(gameDirName);
    if (!LittleFS.exists(fullPath.c_str())) {
      Serial.println("Game directory does not exist");
      return nullptr;
    }

    fullPath += String("/main.lua");
    
    File mainFile = LittleFS.open(fullPath.c_str(), FILE_READ);
    if (!mainFile) {
      Serial.println("Main file does not exist");
      return nullptr;
    }

    char * mainFileData = new char[mainFile.size() + 1];
    mainFile.readBytes(mainFileData, mainFile.size());
    mainFileData[mainFile.size()] = '\0';

    mainFile.close();

    //Serial.println(mainFileData);

    return mainFileData;
  }
}