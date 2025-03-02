#pragma once

#include <Arduino.h>

#define LUA_USE_C89
#include <lua.hpp>

namespace CLuaPhysics {
  void Register(lua_State * state);
}