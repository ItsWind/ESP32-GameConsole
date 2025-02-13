#pragma once

#include <Arduino.h>
#include "Constants.h"
#include "Input.h"
#include "Util.h"
#include "TFTImp.h"
#include "LuaFileStr.h"

#define LUA_USE_C89
#include <lua.hpp>

class LuaImp {
  public:
    lua_State * State;
    void InitializeGame();
    void SendInit();
    void SendUpdate(unsigned long dt);
    void SendDraw();
};