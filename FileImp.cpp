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

  void RemoveFile(const char * filePath) {
    if (LittleFS.remove(filePath)) {
      Serial.println("File removed");
    }
    else {
      Serial.println("File failed to remove");
    }
  }

  void NukeDirectory(const char * dirName) {
    File root = LittleFS.open(dirName);
    if (!root) {
      Serial.println("Directory to nuke not found");
      return;
    }
    if (!root.isDirectory()) {
      Serial.println("Directory to nuke is not directory");
      return;
    }

    File file = root.openNextFile();
    while (file) {
      bool isDirectory = file.isDirectory();
      String filePath = String(file.path());
      file.close();
      Serial.println(filePath.c_str());

      if (isDirectory) {
        Serial.print("DIR FOUND PATH ");
        Serial.println(filePath.c_str());
        NukeDirectory(filePath.c_str());
      }
      else {
        Serial.print("FILE FOUND PATH ");
        Serial.println(filePath.c_str());
        RemoveFile(filePath.c_str());
      }
      file = root.openNextFile();
    }

    root.close();

    if (strcmp(dirName, "/") != 0) {
      Serial.print("REMOVING ROOT ");
      Serial.println(dirName);
      RemoveDirectory(dirName);
    }
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

  char * GetFileData(const char * filePath, uint32_t * getLen) {
    if (!LittleFS.exists(filePath)) {
      Serial.println("File does not exist");
      return nullptr;
    }
    
    File file = LittleFS.open(filePath, FILE_READ);
    if (!file) {
      Serial.println("File cannot be opened");
      return nullptr;
    }
    
    if (getLen != nullptr) {
      *getLen = (uint32_t)(file.size() + 1);
    }

    char * fileData = new char[file.size() + 1];
    file.readBytes(fileData, file.size());
    fileData[file.size()] = '\0';

    file.close();

    return fileData;
  }

  char * GetGameMainData(const char * gameDirName) {
    String fullPath = "/games/" + String(gameDirName) + "/main.lua";
    return GetFileData(fullPath.c_str(), nullptr);
  }
}