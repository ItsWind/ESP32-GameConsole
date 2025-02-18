#pragma once

#include "FS.h"
#include "LittleFS.h"

namespace FileImp {
  void Init();
  void CreateDirectory(const char * dirName);
  void RemoveDirectory(const char * dirName);
}