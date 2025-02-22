#include "LuaImp.h"
#include "Util.h"
#include "Constants.h"
#include "Input.h"
#include "TFTImp.h"
#include "MenuImp.h"
#include "FileImp.h"

static int luaCloseGame(lua_State * state) {
  LuaImp::CloseGame();
  MenuImp::OpenMainMenu();
  return 0;
}

static int luaPrint(lua_State * state) {
  int n = lua_gettop(state);  /* number of arguments */
  int i;
  lua_getglobal(state, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    size_t l;
    lua_pushvalue(state, -1);  /* function to be called */
    lua_pushvalue(state, i);   /* value to print */
    lua_call(state, 1, 1);
    s = lua_tolstring(state, -1, &l);  /* get result */
    if (s == NULL)
      return luaL_error(state, "'tostring' must return a string to 'print'");
    if (i>1) Serial.write("\t");
    Serial.write(s);
    lua_pop(state, 1);  /* pop result */
  }
  Serial.println();
  return 0;
}

static int luaRequire(lua_State * state) {
  // Get string name passed
  const char * reqStrName = lua_tostring(state, 1);

  // Check package.loaded for key entry of reqStrName
  lua_getglobal(state, "package");  // Push 'package' onto the stack
  lua_getfield(state, -1, "loaded"); // Push 'package.loaded' onto the stack
  lua_remove(state, -2);  // Remove 'package' from the stack, leaving 'package.loaded'
  lua_pushstring(state, reqStrName);
  lua_gettable(state, -2);

  // If found in loaded, then return table found
  if (lua_istable(state, -1)) {
    lua_remove(state, -2); // Remove package.loaded
    lua_remove(state, -2); // Remove reqStrName
    return 1;
  }
  // If not found, pop and continue
  lua_pop(state, 1);

  // Check package.preload for key entry of reqStrName
  lua_getglobal(state, "package");  // Push 'package' onto the stack
  lua_getfield(state, -1, "preload"); // Push 'package.preload' onto the stack
  lua_remove(state, -2);  // Remove 'package' from the stack, leaving 'package.preload'
  lua_pushstring(state, reqStrName);
  lua_gettable(state, -2);
  lua_remove(state, -2); // remove preload table, currently: [reqStrName, package.loaded, package.preload.reqStrName]

  if (lua_isfunction(state, -1)) {
    // Execute function if function found
    if (lua_pcall(state, 0, 1, 0) != LUA_OK) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0; // Function execution failed
    }

    // current stack: [reqStrName, package.loaded, returnTable]
    
    int returnsCount = lua_gettop(state) - 2;
    // Check if function executed returned a value
    if (returnsCount != 1) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0;
    }
    // Check if first value is not a table
    else if (!lua_istable(state, 3)) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0;
    }

    // Put new table into loaded
    lua_pushstring(state, reqStrName); // [reqStrName, package.loaded, returnTable, reqStrName]
    lua_insert(state, -2); // [reqStrName, package.loaded, reqStrName, returnTable]
    lua_settable(state, -3); // pops reqStrName at top and returnTable, so we have to get return table back
    // [reqStrName, package.loaded]
    lua_pushstring(state, reqStrName);
    lua_gettable(state, -2);
    lua_remove(state, -2); // remove package.loaded
    lua_remove(state, -2); // remove reqStrName
    
    return 1;
  }
  lua_pop(state, 1); // pop package.preload.reqStrName [reqStrName, package.loaded]
  lua_pop(state, 1); // pop package.loaded [reqStrName]

  // Check file system for file require
  String filePathForRequire = "/games/" + String(LuaImp::CurrentGameDirName) + "/" + String(reqStrName) + ".lua";
  const char * fileData = FileImp::GetFileData(filePathForRequire.c_str());
  if (fileData != nullptr) {
    String loadFileStr = "package.preload[\"" + String(reqStrName) + "\"] = function() " + String(fileData) + " end";
    luaL_dostring(state, loadFileStr.c_str());

    delete[] fileData;

    // Redo require with package.preload.reqStrName loaded
    return luaRequire(state);
  }

  // Nothing found, pop all and return nothing
  lua_pop(state, lua_gettop(state));
  
  return 0;
}

static int luaGetInputVector(lua_State * state) {
  float x = 0.0;
  float y = 0.0;

  uint8_t joystickIndex = (uint8_t)lua_tonumber(state, 1);

  switch (joystickIndex) {
    case 0:
      x = Input::CheckJoystickAxis(JOYSTICK_LEFT_X_PIN) * -1.0;
      y = Input::CheckJoystickAxis(JOYSTICK_LEFT_Y_PIN) * -1.0;
      break;
    case 1:
      x = Input::CheckJoystickAxis(JOYSTICK_RIGHT_X_PIN);
      y = Input::CheckJoystickAxis(JOYSTICK_RIGHT_Y_PIN);
      break;
  }

  lua_pop(state, 1);
  lua_pushnumber(state, x);
  lua_pushnumber(state, y);

  return 2;
}

static int luaGetInputButtonPressed(lua_State * state) {
  bool toReturn = false;

  const char * inputNameToCheck = lua_tostring(state, 1);

  for (uint8_t i = 0; i < sizeof(Input::Buttons) / sizeof(Input::Button); i++) {
    if (strcmp(Input::Buttons[i].inputName, inputNameToCheck) == 0) {
      toReturn = Input::Buttons[i].justPressed;
      break;
    }
  }

  lua_pop(state, 1);
  lua_pushboolean(state, toReturn);

  return 1;
}

static int luaGetInputButtonHeld(lua_State * state) {
  bool toReturn = false;
  unsigned long toReturnHeldFor = 0;

  const char * inputNameToCheck = lua_tostring(state, 1);

  for (uint8_t i = 0; i < sizeof(Input::Buttons) / sizeof(Input::Button); i++) {
    if (strcmp(Input::Buttons[i].inputName, inputNameToCheck) == 0) {
      toReturn = Input::Buttons[i].toggled && !Input::Buttons[i].justPressed;
      toReturnHeldFor = Input::Buttons[i].heldFor;
      break;
    }
  }

  lua_pop(state, 1);
  lua_pushboolean(state, toReturn);
  lua_pushnumber(state, (float)toReturnHeldFor / 1000000.0);

  return 2;
}

static int luaGetInputButtonReleased(lua_State * state) {
  bool toReturn = false;
  unsigned long toReturnHeldFor = 0;

  const char * inputNameToCheck = lua_tostring(state, 1);

  for (uint8_t i = 0; i < sizeof(Input::Buttons) / sizeof(Input::Button); i++) {
    if (strcmp(Input::Buttons[i].inputName, inputNameToCheck) == 0) {
      toReturn = Input::Buttons[i].justReleased;
      toReturnHeldFor = Input::Buttons[i].heldFor;
      break;
    }
  }

  lua_pop(state, 1);
  lua_pushboolean(state, toReturn);
  lua_pushnumber(state, (float)toReturnHeldFor / 1000000.0);

  return 2;
}

static int luaTFTPrint(lua_State * state) {
  const char * str = lua_tostring(state, 1);

  TFTImp::Screen.println(str);

  lua_pop(state, 1);

  return 0;
}

static int luaDrawBox(lua_State * state) {//(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b) {
  int32_t x = (int32_t)lua_tonumber(state, 1);
  int32_t y = (int32_t)lua_tonumber(state, 2);
  int32_t w = (int32_t)lua_tonumber(state, 3);
  int32_t h = (int32_t)lua_tonumber(state, 4);
  uint8_t r = (uint8_t)lua_tonumber(state, 5);
  uint8_t g = (uint8_t)lua_tonumber(state, 6);
  uint8_t b = (uint8_t)lua_tonumber(state, 7);

  TFTImp::DrawBox(x, y, w, h, Util::rgb888_to_rgb565(r, g, b));

  lua_pop(state, 7);

  return 0;
}

namespace LuaImp {
  lua_State * State = nullptr;
  const char * CurrentGameDirName = nullptr;

  void CloseGame() {
    if (State == nullptr) {
      return;
    }

    lua_pop(State, lua_gettop(State));
    lua_close(State);
    State = nullptr;
    CurrentGameDirName = nullptr;
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

    // Load base libs
    luaopen_base(State);
    luaopen_table(State);
    luaopen_string(State);
    luaopen_math(State);

    luaL_dostring(State, "package = {}\npackage.preload = {}\npackage.loaded = {}");

    luaL_dostring(State, "fool = {}\nfool.Init = function() end\nfool.Update = function(dt) end\nfool.FixedUpdate = function(dt) end\nfool.Draw = function() end");

    lua_register(State, "closeGame", luaCloseGame);
    lua_register(State, "print", luaPrint);
    lua_register(State, "require", luaRequire);
    lua_register(State, "getInputVector", luaGetInputVector);
    lua_register(State, "getInputButtonPressed", luaGetInputButtonPressed);
    lua_register(State, "getInputButtonHeld", luaGetInputButtonHeld);
    lua_register(State, "getInputButtonReleased", luaGetInputButtonReleased);
    lua_register(State, "tftPrint", luaTFTPrint);
    lua_register(State, "drawBox", luaDrawBox);

    // Seed random
    String mathRandomSeedStr = "math.randomseed(" + String(random(1, 10000000)) + ")";
    luaL_dostring(State, mathRandomSeedStr.c_str());

    // Do game main data
    luaL_dostring(State, gameMainData);
    delete[] gameMainData;

    SendInit();
  }

  void SendInit() {
    lua_getglobal(State, "fool");
    lua_getfield(State, -1, "Init");
    lua_remove(State, -2);
    //lua_getglobal(State, "Init");
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
    //lua_getglobal(State, "Update");
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
    //lua_getglobal(State, "FixedUpdate");
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
    //lua_getglobal(State, "Draw");
    if (lua_pcall(State, 0, 0, 0) != LUA_OK) {
      const char * errMsg = lua_tostring(State, -1);

      Serial.println("error calling draw");
      Serial.println(errMsg);
      while (true) {}
    }
  }
}