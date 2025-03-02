#pragma once

#include <Arduino.h>

#define LUA_USE_C89
#include <lua.hpp>

namespace CLuaDraw {
  void Register(lua_State * state);
  void Close();
}