#pragma once

#include <Arduino.h>

#define LUA_USE_C89
#include <lua.hpp>

namespace CLuaSys {
  void Register(lua_State * state);
}