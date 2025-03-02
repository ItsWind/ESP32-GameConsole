#include "src/Imps/MenuImp.h"
#include "Input.h"
#include "src/Imps/FileImp.h"
#include "src/Imps/TFTImp.h"
#include "src/Imps/NetImp.h"
#include "src/Imps/LuaImp.h"
#include "src/FIMG/FoolFIMG.h"

namespace MenuImp {
  Menu * CurrentMenu = nullptr;

  // BASE MENU
  void Menu::Init() {}
  void Menu::Destroy() {}
  void Menu::Update(unsigned long dt) {}
  void Menu::Draw() {}

  // SPLASH MENU
  void SplashMenu::Init() {
    screenTime = 0;
  }
  void SplashMenu::Destroy() {}
  void SplashMenu::Update(unsigned long dt) {
    screenTime += dt;
    if (screenTime >= 3000000) {
      SetMenu(new MainMenu());
    }
  }
  void SplashMenu::Draw() {
    //int16_t alphaOffset = screenTime <= 2000000 ? (int16_t)lerp(-255.0, 0.0, (float)(screenTime / 2000000)) : screenTime >= 2800000 ? (int16_t)lerp(0.0, -255.0, (float)((screenTime - 2800000) / (200000))) : 0;
    uint8_t alphaOffset = 0;
    if (screenTime <= 1000000) {
      float lerpVal = (float)screenTime / 1000000.0;
      alphaOffset = (uint8_t)round(lerp(255.0, 0.0, lerpVal));
    }
    else if (screenTime >= 2000000) {
      float lerpVal = (float)(screenTime - 2000000) / 1000000.0;
      alphaOffset = (uint8_t)round(lerp(0.0, 255.0, lerpVal));
    }
    TFTImp::DrawFIMG(48, 32, false, alphaOffset, FOOL_FIMG, sizeof(FOOL_FIMG));
  }

  // MESSAGE MENU
  MessageMenu::MessageMenu(const char * message) {
    messageToDisplay = message;
  }
  void MessageMenu::Init() {}
  void MessageMenu::Destroy() {}
  void MessageMenu::Update(unsigned long dt) {
    if (Input::Buttons[0].justPressed) {
      SetMenu(new MainMenu());
    }
  }
  void MessageMenu::Draw() {
    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText(messageToDisplay);
    TFTImp::DrawCenteredText(TFTImp::Screen.width() / 2, TFTImp::Screen.height() / 2 + 16, "OK -> Main Menu");
  }

  // MAIN MENU
  void MainMenu::Init() {}
  void MainMenu::Destroy() {}
  void MainMenu::Update(unsigned long dt) {
    if (Input::Buttons[0].justPressed) {
      LuaImp::InitializeGame("test");

      SetMenu(nullptr);
    }
    else if (Input::Buttons[1].justPressed) {
      NetImp::StartGameDownload(1);
    }
  }
  void MainMenu::Draw() {
    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText("Hello!");
  }
  
  void SetMenu(Menu * newMenu) {
    if (CurrentMenu != nullptr) {
      CurrentMenu->Destroy();
      delete CurrentMenu;
    }

    CurrentMenu = newMenu;

    if (CurrentMenu != nullptr) {
      CurrentMenu->Init();
    }
  }
}