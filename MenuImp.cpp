#include "MenuImp.h"
#include "Input.h"
#include "FileImp.h"
#include "TFTImp.h"
#include "NetImp.h"
#include "LuaImp.h"

namespace MenuImp {
  Menu * CurrentMenu = nullptr;//new Menu();

  // BASE MENU
  void Menu::Init() {}
  void Menu::Destroy() {}
  void Menu::Update(unsigned long dt) {}
  void Menu::Draw() {}

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