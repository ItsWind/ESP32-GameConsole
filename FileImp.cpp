#include "FileImp.h"

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
}