#pragma once

#include <Arduino.h>

#define LUA_USE_C89
#include <lua.hpp>

namespace LuaImp {
  extern lua_State * State;
  extern const char * CurrentGameDirName;
  
  void CloseGame();
  void InitializeGame(const char * gameDirName);
  void SendInit();
  void SendUpdate(unsigned long dt);
  void SendFixedUpdate(unsigned long dt);
  void SendDraw();
}