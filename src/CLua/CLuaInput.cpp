#include "src/CLua/CLuaInput.h"
#include "Constants.h"
#include "Input.h"

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

namespace CLuaInput {
  void Register(lua_State * state) {
    lua_register(state, "getInputVector", luaGetInputVector);
    lua_register(state, "getInputButtonPressed", luaGetInputButtonPressed);
    lua_register(state, "getInputButtonHeld", luaGetInputButtonHeld);
    lua_register(state, "getInputButtonReleased", luaGetInputButtonReleased);
  }
}