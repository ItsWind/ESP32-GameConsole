#include <Arduino.h>
#include "Util.h"
#include "Constants.h"
#include "Input.h"
#include "TFTImp.h"
#include "FileImp.h"
#include "LuaImp.h"

void initPins() {
  // Left (drive) stick
  pinMode(JOYSTICK_LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_LEFT_X_PIN, INPUT);
  pinMode(JOYSTICK_LEFT_Y_PIN, INPUT);

  // Right (turn) stick
  pinMode(JOYSTICK_RIGHT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_X_PIN, INPUT);
  pinMode(JOYSTICK_RIGHT_Y_PIN, INPUT);

  pinMode(DPAD_CENTER_PIN, INPUT_PULLUP);
  pinMode(DPAD_UP_PIN, INPUT_PULLUP);
  pinMode(DPAD_RIGHT_PIN, INPUT_PULLUP);
  pinMode(DPAD_DOWN_PIN, INPUT_PULLUP);
  pinMode(DPAD_LEFT_PIN, INPUT_PULLUP);
}

// Time variables for loop (SET AT END OF setup)
unsigned long oldTime = 0;
void setup() {
  Serial.begin(115200);
  delay(1000);

  FileImp::Init();

  initPins();

  TFTImp::Init();

  LuaImp::InitializeGame();

  LuaImp::SendInit();

  // SET LOOP TIME AT END OF setup
  oldTime = micros();
}

const uint16_t FIXED_UPDATE_TIME_NEEDED = 33333; // 16667
unsigned long fixedUpdateTimer = 0;
void loop() {
  unsigned long thisTime = micros();
  unsigned long dt = 0;
  // Handle overflow
  if (thisTime < oldTime) {
    unsigned long maxNum = 0;
    maxNum--;

    dt = (maxNum - oldTime) + thisTime + 1;
  }
  else {
    dt = thisTime - oldTime;
  }
  oldTime = thisTime;

  fixedUpdateTimer += dt;
  
  Input::CheckButtonInputs(dt);

  LuaImp::SendUpdate(dt);

  while (fixedUpdateTimer >= FIXED_UPDATE_TIME_NEEDED) {
    LuaImp::SendFixedUpdate(FIXED_UPDATE_TIME_NEEDED);
    fixedUpdateTimer -= FIXED_UPDATE_TIME_NEEDED;
  }

  TFTImp::PrepareNewFrameSprite();
  LuaImp::SendDraw();
  TFTImp::PushCurrentFrameSprite(dt);
}
