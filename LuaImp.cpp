#include "LuaImp.h"

extern "C" {
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

  static int luaGetInputVector(lua_State * state) {
    float x = 0.0;
    float y = 0.0;

    uint8_t joystickIndex = (uint8_t)lua_tonumber(state, 1);

    switch (joystickIndex) {
      case 0:
        x = checkJoystickAxis(JOYSTICK_LEFT_X_PIN) * -1.0;
        y = checkJoystickAxis(JOYSTICK_LEFT_Y_PIN) * -1.0;
        break;
      case 1:
        x = checkJoystickAxis(JOYSTICK_RIGHT_X_PIN);
        y = checkJoystickAxis(JOYSTICK_RIGHT_Y_PIN);
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

    for (uint8_t i = 0; i < sizeof(buttons) / sizeof(Button); i++) {
      if (strcmp(buttons[i].inputName, inputNameToCheck) == 0) {
        toReturn = buttons[i].justPressed;
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

    for (uint8_t i = 0; i < sizeof(buttons) / sizeof(Button); i++) {
      if (strcmp(buttons[i].inputName, inputNameToCheck) == 0) {
        toReturn = buttons[i].toggled && !buttons[i].justPressed;
        toReturnHeldFor = buttons[i].heldFor;
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

    for (uint8_t i = 0; i < sizeof(buttons) / sizeof(Button); i++) {
      if (strcmp(buttons[i].inputName, inputNameToCheck) == 0) {
        toReturn = buttons[i].justReleased;
        toReturnHeldFor = buttons[i].heldFor;
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

    tft.println(str);

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

    tftFrameSprite.fillRect(x - (w/2), y - (h/2), w, h, rgb888_to_rgb565(r, g, b));

    lua_pop(state, 7);

    return 0;
  }
}

void LuaImp::InitializeGame() {
  State = luaL_newstate();

  luaopen_base(State);
  luaopen_table(State);
  luaopen_string(State);
  luaopen_math(State);

  lua_register(State, "print", luaPrint);
  lua_register(State, "getInputVector", luaGetInputVector);
  lua_register(State, "getInputButtonPressed", luaGetInputButtonPressed);
  lua_register(State, "getInputButtonHeld", luaGetInputButtonHeld);
  lua_register(State, "getInputButtonReleased", luaGetInputButtonReleased);
  lua_register(State, "tftPrint", luaTFTPrint);
  lua_register(State, "drawBox", luaDrawBox);

  Serial.println("Hello?");
  String mathRandomSeed = "math.randomseed(" + String(random(1, 10000000)) + ")\r\n";
  luaL_dostring(State, mathRandomSeed.c_str());
  Serial.print(luaL_dostring(State, LUA_FILE_STR));
}

void LuaImp::SendInit() {
  lua_getglobal(State, "init");
  if (lua_pcall(State, 0, 0, 0) != LUA_OK) {
    Serial.println("error calling init");
  }
}

void LuaImp::SendUpdate(unsigned long dt) {
  lua_getglobal(State, "update");
  lua_pushnumber(State, (double)dt / 1000000.0);
  if (lua_pcall(State, 1, 0, 0) != LUA_OK) {
    Serial.println("error calling update");
  }
}

void LuaImp::SendDraw() {
  lua_getglobal(State, "draw");
  if (lua_pcall(State, 0, 0, 0) != LUA_OK) {
    Serial.println("error calling draw");
  }
}