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
  extern Button Buttons[9];

  void SetVibration(uint8_t vibrationPwm);
  uint8_t GetVibration();
  void CheckButtonInputs(unsigned long dt);
  float CheckJoystickAxis(uint8_t pinNumber);
}