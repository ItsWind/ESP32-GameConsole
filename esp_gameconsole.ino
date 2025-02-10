#include <LuaWrapper.h>
#include "LuaFileStr.h"
#include <TFT_eSPI.h>
#include <SPI.h>

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

/////////////////////////////////////////////////////////// CONSTANTS

const uint8_t JOYSTICK_LEFT_X_PIN = 34;
const uint8_t JOYSTICK_LEFT_Y_PIN = 35;
const uint8_t JOYSTICK_LEFT_BUTTON_PIN = 27;
const uint8_t JOYSTICK_RIGHT_X_PIN = 36;
const uint8_t JOYSTICK_RIGHT_Y_PIN = 39;
const uint8_t JOYSTICK_RIGHT_BUTTON_PIN = 4;
const uint8_t DPAD_CENTER_PIN = 22;
const uint8_t DPAD_UP_PIN = 32;
const uint8_t DPAD_RIGHT_PIN = 33;
const uint8_t DPAD_DOWN_PIN = 14;
const uint8_t DPAD_LEFT_PIN = 13;

const uint16_t JOYSTICK_DEADZONE_UPPER = 2200;
const uint16_t JOYSTICK_DEADZONE_LOWER = 1500;

const uint16_t BUTTON_DEBOUNCE_MICROS = 20000;

/////////////////////////////////////////////////////////// OBJECTS

LuaWrapper lua;
TFT_eSPI tft = TFT_eSPI();

/////////////////////////////////////////////////////////// STRUCTS

struct Button {
  uint8_t pinNum;
  unsigned long heldFor;
  bool toggled;
  const char * inputName;
};
Button buttons[] = {
  {DPAD_CENTER_PIN, 0, false, "BUTTON_CENTER"},
  {DPAD_UP_PIN, 0, false, "BUTTON_UP"},
  {DPAD_RIGHT_PIN, 0, false, "BUTTON_RIGHT"},
  {DPAD_DOWN_PIN, 0, false, "BUTTON_DOWN"},
  {DPAD_LEFT_PIN, 0, false, "BUTTON_LEFT"},
  {JOYSTICK_LEFT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_LEFT"},
  {JOYSTICK_RIGHT_BUTTON_PIN, 0, false, "BUTTON_JOYSTICK_RIGHT"}
};

/////////////////////////////////////////////////////////// UTIL FUNCTIONS

uint16_t rgb888_to_rgb565(uint8_t R, uint8_t G, uint8_t B) {
  uint16_t r_565 = (R >> 3) & 0x1F; // Red component
  uint16_t g_565 = (G >> 2) & 0x3F; // Green component
  uint16_t b_565 = (B >> 3) & 0x1F; // Blue component
  
  return (r_565 << 11) | (g_565 << 5) | b_565;
}

/////////////////////////////////////////////////////////// LUA -> C FUNCTIONS

int luaDoTFTPrintLn(lua_State * state) {
  const char * str = lua_tostring(state, 1);

  if (str != NULL) {
    tft.println(str);
  }

  lua_pop(state, 1);

  return 0;
}

/////////////////////////////////////////////////////////// C -> LUA FUNCTIONS

void luaSendInit() {
  lua_getglobal(lua.State, "init");
  if (lua_pcall(lua.State, 0, 0, 0) != LUA_OK) {
    Serial.println("error calling init");
  }
}

void luaSendUpdate(unsigned long dt) {
  lua_getglobal(lua.State, "update");
  lua_pushnumber(lua.State, (float)dt / 1000000.0);
  if (lua_pcall(lua.State, 1, 0, 0) != LUA_OK) {
    Serial.println("error calling update");
  }
}

void luaSendInputEvent(const char * inputName, const char * inputType, float inputValue, float inputValue2) {
  lua_getglobal(lua.State, "input");
  lua_pushstring(lua.State, inputName);
  lua_pushstring(lua.State, inputType);
  lua_pushnumber(lua.State, inputValue);
  lua_pushnumber(lua.State, inputValue2);
  if (lua_pcall(lua.State, 4, 0, 0) != LUA_OK) {
    Serial.println("error calling input");
  }
}

/////////////////////////////////////////////////////////// CORE FUNCTIONS

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

void initLua() {
  lua_register(lua.State, "tftPrintLn", luaDoTFTPrintLn);

  Serial.println("Hello?");
  Serial.print(lua.Lua_dostring(&LUA_FILE_STR));
}

void checkButtonInputs(unsigned long dt) {
  // Iterate button pins
  for (int i = 0; i < sizeof(buttons) / sizeof(Button); i++) {
    uint8_t pinNum = buttons[i].pinNum;
    int buttonState = digitalRead(pinNum);

    // If button pressed (LOW)
    if (buttonState == LOW) {
      //if (buttons[i].toggled == false && buttons[i].heldFor >= 20000) {
      if (buttons[i].heldFor >= BUTTON_DEBOUNCE_MICROS) {
        const char * inputType = nullptr;
        // Toggled on first time after being off
        if (!buttons[i].toggled) {
          buttons[i].toggled = true;
          inputType = "PRESSED";
        }
        else {
          inputType = "HELD";
        }

        luaSendInputEvent(buttons[i].inputName, inputType, (float)(buttons[i].heldFor - BUTTON_DEBOUNCE_MICROS) / 1000000.0, 0);
      }
      buttons[i].heldFor += dt;
    }
    else {
      if (buttons[i].toggled) {
        luaSendInputEvent(buttons[i].inputName, "RELEASED", (float)(buttons[i].heldFor - BUTTON_DEBOUNCE_MICROS) / 1000000.0, 0);
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

void checkJoystickInputs() {
  float leftX = checkJoystickAxis(JOYSTICK_LEFT_X_PIN);
  float leftY = checkJoystickAxis(JOYSTICK_LEFT_Y_PIN);
  if (leftX != 0.0 || leftY != 0.0) {
    luaSendInputEvent("JOYSTICK_LEFT", "MOVED", leftX * -1.0, leftY);
  }

  float rightX = checkJoystickAxis(JOYSTICK_RIGHT_X_PIN);
  float rightY = checkJoystickAxis(JOYSTICK_RIGHT_Y_PIN);
  if (rightX != 0.0 || rightY != 0.0) {
    luaSendInputEvent("JOYSTICK_RIGHT", "MOVED", rightX, rightY * -1.0);
  }
}

// Time variables for loop (SET AT END OF setup)
unsigned long oldTime = 0;
void setup() {
  Serial.begin(115200);

  initPins();

  initLua();

  tft.init();
  tft.setRotation(45);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setCursor(0, 0);

  tft.println("DISPLAY STARTED");

  // Call lua init
  luaSendInit();

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
  
  // Handle lua input sending
  checkButtonInputs(dt);
  checkJoystickInputs();

  // Call lua update with dt
  luaSendUpdate(dt);
}
