#pragma once

#include <Arduino.h>

namespace FileImp {
  void Init();
  void CreateDirectory(const char * dirName);
  void RemoveDirectory(const char * dirName);
  void RemoveFile(const char * filePath);
  void NukeDirectory(const char * dirName);
  bool AppendBytesToGameFile(const char * dirAndFileName, const uint8_t * bytes, uint length);
  char * GetFileData(const char * filePath);
  char * GetGameMainData(const char * gameDirName);
}