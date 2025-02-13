#include "Input.h"

Button buttons[7] = {
  {DPAD_CENTER_PIN, 0, false, "BUTTON_CENTER"},
  {DPAD_UP_PIN, 0, false, "BUTTON_UP"},
  {DPAD_RIGHT_PIN, 0, false, "BUTTON_RIGHT"},
  {DPAD_DOWN_PIN, 0, false, "BUTTON_DOWN"},
  {DPAD_LEFT_PIN, 0, false, "BUTTON_LEFT"},
  {JOYSTICK_LEFT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_LEFT"},
  {JOYSTICK_RIGHT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_RIGHT"}
};

void checkButtonInputs(unsigned long dt) {
  // Iterate button pins
  for (uint8_t i = 0; i < sizeof(buttons) / sizeof(Button); i++) {
    uint8_t pinNum = buttons[i].pinNum;
    int buttonState = digitalRead(pinNum);

    // If button pressed (LOW)
    if (buttonState == LOW) {
      if (buttons[i].heldFor >= BUTTON_DEBOUNCE_MICROS) {
        // Toggled on first time after being off
        if (!buttons[i].toggled) {
          buttons[i].toggled = true;
          buttons[i].justReleased = false;
          buttons[i].justPressed = true;
        }
        else {
          buttons[i].justPressed = false;
        }
      }
      buttons[i].heldFor += dt;
    }
    else {
      if (buttons[i].toggled) {
        buttons[i].justPressed = false;
        buttons[i].justReleased = true;
      }
      else {
        buttons[i].justReleased = false;
      }

      buttons[i].toggled = false;
      buttons[i].heldFor = 0;
    }
  }
}

float checkJoystickAxis(uint8_t pinNumber) {
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