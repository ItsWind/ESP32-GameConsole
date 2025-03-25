#pragma once

#include <Arduino.h>

const uint8_t SPEAKER_DOUT = 12;
const uint8_t SPEAKER_BCLK = 26;
const uint8_t SPEAKER_WSLRC = 25;

const uint8_t JOYSTICK_LEFT_X_PIN = 34;
const uint8_t JOYSTICK_LEFT_Y_PIN = 35;

const uint8_t JOYSTICK_RIGHT_X_PIN = 36;
const uint8_t JOYSTICK_RIGHT_Y_PIN = 39;

const uint8_t DPAD_CENTER_PIN = 22;
const uint8_t DPAD_UP_PIN = 32;
const uint8_t DPAD_RIGHT_PIN = 33;
const uint8_t DPAD_DOWN_PIN = 14;
const uint8_t DPAD_LEFT_PIN = 13;
const uint8_t JOYSTICK_LEFT_BUTTON_PIN = 27;
const uint8_t JOYSTICK_RIGHT_BUTTON_PIN = 4;
const uint8_t LEFT_BUMPER_BUTTON_PIN = 21;
const uint8_t RIGHT_BUMPER_BUTTON_PIN = 19;

const uint8_t SCREEN_DIM_PIN = 15;

const uint8_t VIBRATION_PIN = 5;

const uint16_t JOYSTICK_DEADZONE_UPPER = 2100;
const uint16_t JOYSTICK_DEADZONE_LOWER = 1500;

const uint16_t BUTTON_DEBOUNCE_MICROS = 20000;