#pragma once

#include <Arduino.h>

namespace FileImp {
  void Init();
  void CreateDirectory(const char * dirName);
  void RemoveDirectory(const char * dirName);
  bool AppendBytesToGameFile(const char * dirAndFileName, const uint8_t * bytes, uint length);
}