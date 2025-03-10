#pragma once

#include <Arduino.h>

namespace Input {
  struct Button {
    uint8_t pinNum;
    unsigned long heldFor;
    bool toggled;
    const char * inputName;

    bool justPressed;
    bool justReleased;
  };
  extern Button Buttons[7];

  extern void CheckButtonInputs(unsigned long dt);
  extern float CheckJoystickAxis(uint8_t pinNumber);
}