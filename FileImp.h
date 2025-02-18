#pragma once

#include <Arduino.h>

namespace FileImp {
  void Init();
  void CreateDirectory(const char * dirName);
  void RemoveDirectory(const char * dirName);
}