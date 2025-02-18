#include "MenuImp.h"
#include "Input.h"
#include "FileImp.h"
#include "TFTImp.h"
#include "LuaImp.h"

namespace MenuImp {
  Menu * CurrentMenu = nullptr;//new Menu();

  // BASE MENU
  void Menu::Init() {}
  void Menu::Destroy() {}
  void Menu::Update(unsigned long dt) {}
  void Menu::Draw() {}

  // MAIN MENU
  void MainMenu::Init() {}
  void MainMenu::Destroy() {}
  void MainMenu::Update(unsigned long dt) {
    if (Input::Buttons[0].justPressed) {
      LuaImp::InitializeGame();

      SetMenu(nullptr);
    }
  }
  void MainMenu::Draw() {
    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText(TFTImp::Screen.width() / 2, TFTImp::Screen.height() / 2, "Hello!");
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

  void OpenMainMenu() {
    SetMenu(new MainMenu());
  }
}