#include "src/Imps/LuaImp.h"
#include "Constants.h"
#include "src/Imps/FileImp.h"

#include "src/CLua/CLuaSys.h"
#include "src/CLua/CLuaDraw.h"
#include "src/CLua/CLuaInput.h"

namespace LuaImp {
  lua_State * State = nullptr;
  const char * CurrentGameDirName = nullptr;

  void CloseGame() {
    if (State == nullptr) {
      return;
    }

    lua_close(State);
    State = nullptr;
    CurrentGameDirName = nullptr;

    CLuaDraw::Close();
  }

  void InitializeGame(const char * gameDirName) {
    if (State != nullptr) {
      return;
    }

    const char * gameMainData = FileImp::GetGameMainData(gameDirName);
    if (gameMainData == nullptr) {
      return;
    }

    CurrentGameDirName = gameDirName;
    State = luaL_newstate();

    // Set garbage collector to collect more often, MCU low ram :(
    lua_gc(State, LUA_GCSETPAUSE, 100);

    // Load base libs
    luaopen_base(State);
    luaopen_table(State);
    luaopen_string(State);
    luaopen_math(State);

    // Load global tables
    luaL_dostring(State, R""""(
      package = {}
      package.preload = {}
      package.loaded = {}

      fool = {}
      fool.Init = function() end
      fool.Update = function(dt) end
      fool.FixedUpdate = function(dt) end
      fool.Draw = function() end
    )"""");

    CLuaSys::Register(State);
    CLuaInput::Register(State);
    CLuaDraw::Register(State);

    // Seed random
    lua_getglobal(State, "math");
    lua_getfield(State, -1, "randomseed");
    lua_remove(State, -2);
    lua_pushnumber(State, random(1, 10000000));
    lua_call(State, 1, 0);

    // Do game main data
    luaL_dostring(State, gameMainData);
    delete[] gameMainData;

    SendInit();
  }

  void SendInit() {
    lua_getglobal(State, "fool");
    lua_getfield(State, -1, "Init");
    lua_remove(State, -2);

    if (lua_pcall(State, 0, 0, 0) != LUA_OK) {
      const char * errMsg = lua_tostring(State, -1);

      Serial.println("error calling init");
      Serial.println(errMsg);
      while (true) {}
    }
  }

  void SendUpdate(unsigned long dt) {
    lua_getglobal(State, "fool");
    lua_getfield(State, -1, "Update");
    lua_remove(State, -2);

    lua_pushnumber(State, (double)dt / 1000000.0);
    if (lua_pcall(State, 1, 0, 0) != LUA_OK) {
      const char * errMsg = lua_tostring(State, -1);

      Serial.println("error calling update");
      Serial.println(errMsg);
      while (true) {}
    }
  }

  void SendFixedUpdate(unsigned long dt) {
    lua_getglobal(State, "fool");
    lua_getfield(State, -1, "FixedUpdate");
    lua_remove(State, -2);

    lua_pushnumber(State, (double)dt / 1000000.0);
    if (lua_pcall(State, 1, 0, 0) != LUA_OK) {
      const char * errMsg = lua_tostring(State, -1);

      Serial.println("error calling fixed update");
      Serial.println(errMsg);
      while (true) {}
    }
  }

  void SendDraw() {
    lua_getglobal(State, "fool");
    lua_getfield(State, -1, "Draw");
    lua_remove(State, -2);

    if (lua_pcall(State, 0, 0, 0) != LUA_OK) {
      const char * errMsg = lua_tostring(State, -1);

      Serial.println("error calling draw");
      Serial.println(errMsg);
      while (true) {}
    }
  }
}