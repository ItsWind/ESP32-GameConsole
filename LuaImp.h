#pragma once

#include <Arduino.h>
#include "Util.h"
#include "Constants.h"
#include "Input.h"
#include "TFTImp.h"
#include "SimpleCollisionsFileStr.h"
#include "LuaFileStr.h"

#define LUA_USE_C89
#include <lua.hpp>

namespace LuaImp {
  extern lua_State * State;
  
  void InitializeGame();
  void SendInit();
  void SendUpdate(unsigned long dt);
  void SendFixedUpdate(unsigned long dt);
  void SendDraw();
}