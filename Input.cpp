#include "Input.h"
#include "Constants.h"

static uint8_t lastVibrationSet = 0;

namespace Input {
  Button Buttons[9] = {
    {DPAD_CENTER_PIN, 0, false, "BUTTON_CENTER", false, false},
    {DPAD_UP_PIN, 0, false, "BUTTON_UP", false, false},
    {DPAD_RIGHT_PIN, 0, false, "BUTTON_RIGHT", false, false},
    {DPAD_DOWN_PIN, 0, false, "BUTTON_DOWN", false, false},
    {DPAD_LEFT_PIN, 0, false, "BUTTON_LEFT", false, false},
    {JOYSTICK_LEFT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_LEFT", false, false},
    {JOYSTICK_RIGHT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_RIGHT", false, false},
    {LEFT_BUMPER_BUTTON_PIN, 0, false, "BUTTON_BUMPER_LEFT", false, false},
    {RIGHT_BUMPER_BUTTON_PIN, 0, false, "BUTTON_BUMPER_RIGHT", false, false}
  };

  void SetVibration(uint8_t vibrationPwm) {
    if (lastVibrationSet == vibrationPwm) {
      return;
    }

    analogWrite(VIBRATION_PIN, vibrationPwm);
    lastVibrationSet = vibrationPwm;
  }

  uint8_t GetVibration() {
    return lastVibrationSet;
  }

  void CheckButtonInputs(unsigned long dt) {
    // Iterate button pins
    for (uint8_t i = 0; i < sizeof(Buttons) / sizeof(Button); i++) {
      uint8_t pinNum = Buttons[i].pinNum;
      int buttonState = digitalRead(pinNum);

      // If button pressed (LOW)
      if (buttonState == LOW) {
        if (Buttons[i].heldFor >= BUTTON_DEBOUNCE_MICROS) {
          // Toggled on first time after being off
          if (!Buttons[i].toggled) {
            Buttons[i].toggled = true;
            Buttons[i].justReleased = false;
            Buttons[i].justPressed = true;
          }
          else {
            Buttons[i].justPressed = false;
          }
        }
        Buttons[i].heldFor += dt;
      }
      else {
        if (Buttons[i].toggled) {
          Buttons[i].justPressed = false;
          Buttons[i].justReleased = true;
        }
        else {
          Buttons[i].justReleased = false;
        }

        Buttons[i].toggled = false;
        Buttons[i].heldFor = 0;
      }
    }
  }

  float CheckJoystickAxis(uint8_t pinNumber) {
    float toReturn = 0.0;
    uint16_t joystickAxisRead = analogRead(pinNumber);

    if (joystickAxisRead <= JOYSTICK_DEADZONE_LOWER) {
      toReturn = lerp(-1.0, 0.0, (float)joystickAxisRead / (float)JOYSTICK_DEADZONE_LOWER);
    }
    else if (joystickAxisRead >= JOYSTICK_DEADZONE_UPPER) {
      toReturn = lerp(0.0, 1.0, (float)(joystickAxisRead - JOYSTICK_DEADZONE_UPPER) / (float)(4095 - JOYSTICK_DEADZONE_UPPER));
    }

    return toReturn;
  }
}