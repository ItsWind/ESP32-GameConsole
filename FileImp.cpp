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
}