#pragma once

#include "Constants.h"

struct Button {
  uint8_t pinNum;
  unsigned long heldFor;
  bool toggled;
  const char * inputName;

  bool justPressed = false;
  bool justReleased = false;
};
extern Button buttons[7];

extern void checkButtonInputs(unsigned long dt);
extern float checkJoystickAxis(uint8_t pinNumber);