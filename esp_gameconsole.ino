#include <Arduino.h>
#include "Constants.h"
#include "Input.h"
#include "TFTImp.h"
#include "LuaImp.h"

LuaImp lua;

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

  initPins();

  tft.init();
  tft.setRotation(45);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextWrap(false);
  tft.setCursor(0, 0);

  tft.println("DISPLAY STARTED");

  lua.InitializeGame();

  lua.SendInit();

  // SET LOOP TIME AT END OF setup
  oldTime = micros();
}

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
  
  checkButtonInputs(dt);

  lua.SendUpdate(dt);

  tftFrameSprite.deleteSprite();
  tftFrameSprite = TFT_eSprite(&tft);
  tftFrameSprite.createSprite(tft.width(), tft.height());
  tftFrameSprite.fillSprite(TFT_BLACK);
  lua.SendDraw();
  tftFrameSprite.pushSprite(0, 0);
}
