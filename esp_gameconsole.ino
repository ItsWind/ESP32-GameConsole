#include <Arduino.h>
#include "NetImp.h"
#include "Util.h"
#include "Constants.h"
#include "Input.h"
#include "TFTImp.h"
#include "MenuImp.h"
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
  initPins();

  TFTImp::Init();

  TFTImp::Screen.println("Establishing wifi connection");
  NetImp::Init();

  TFTImp::Screen.println("Beginning serial");
  Serial.begin(115200);

  TFTImp::Screen.println("Beginning file directory");
  FileImp::Init();

  TFTImp::Screen.println("Opening main menu");
  MenuImp::SetMenu(new MenuImp::MainMenu());

  // SET LOOP TIME AT END OF setup
  oldTime = micros();
}

const uint16_t FIXED_UPDATE_TIME_NEEDED = 33333; // 16667
unsigned long fixedUpdateTimer = 0;
void loop() {
  uint32_t currentHeapFree = heap_caps_get_free_size(MALLOC_CAP_8BIT);
  if (currentHeapFree <= 10000 && LuaImp::State != nullptr) {
    LuaImp::CloseGame();
    MenuImp::SetMenu(new MenuImp::MessageMenu("RAM drained. Exited."));
  }
  Serial.println(currentHeapFree);

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

  NetImp::Update(dt);
  
  Input::CheckButtonInputs(dt);

  if (LuaImp::State != nullptr) {
    fixedUpdateTimer += dt;

    LuaImp::SendUpdate(dt);

    // Check if game was closed in Update() or a previous FixedUpdate()
    while (LuaImp::State != nullptr && fixedUpdateTimer >= FIXED_UPDATE_TIME_NEEDED) {
      LuaImp::SendFixedUpdate(FIXED_UPDATE_TIME_NEEDED);
      fixedUpdateTimer -= FIXED_UPDATE_TIME_NEEDED;
    }
  }
  else if (MenuImp::CurrentMenu != nullptr) {
    if (!NetImp::DownloadingGame) {
      MenuImp::CurrentMenu->Update(dt);
    }
  }

  TFTImp::PrepareNewFrameSprite();

  if (LuaImp::State != nullptr) {
    LuaImp::SendDraw();
  }
  else if (MenuImp::CurrentMenu != nullptr) {
    if (NetImp::DownloadingGame) {
      NetImp::Draw();
    }
    else {
      MenuImp::CurrentMenu->Draw();
    }
  }

  TFTImp::PushCurrentFrameSprite(dt);
}
