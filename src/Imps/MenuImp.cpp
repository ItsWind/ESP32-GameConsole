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
    if (Input::Buttons[1].justPressed) {
      SetMenu(new InstallMenu());
    }
    else if (Input::Buttons[3].justPressed) {
      SetMenu(new GamesMenu());
    }
  }
  void MainMenu::Draw() {
    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText("Hello!");
  }

  // TEXTLIST MENU
  void TextListMenu::Init() {
    currentTextIndex = 0;
  }
  void TextListMenu::Destroy() {
    for (uint8_t i = 0; i < textListCount; i++) {
      delete[] textList[i];
    }
    delete[] textList;
  }
  void TextListMenu::Update(unsigned long dt) {
    if (Input::Buttons[4].justPressed) {
      SetMenu(new MainMenu());
    }
    
    if (textListCount == 0) {
      return;
    }

    if (Input::Buttons[1].justPressed) {
      if (currentTextIndex == 0) {
        currentTextIndex = textListCount - 1;
      }
      else {
        currentTextIndex--;
      }
    }
    else if (Input::Buttons[3].justPressed) {
      if (currentTextIndex == textListCount - 1) {
        currentTextIndex = 0;
      }
      else {
        currentTextIndex++;
      }
    }
  }
  void TextListMenu::Draw() {
    TFTImp::FrameSprite.fillSprite(TFT_BLUE);

    if (textListCount > 0) {
      for (uint8_t i = 0; i < textListCount; i++) {
        int32_t spacer = 4;
        int32_t h = 16;
        int32_t w = TFTImp::Screen.width() - (spacer * 2);
        int32_t y = spacer + ((h + spacer) * (int32_t)i);
        
        uint16_t colorForRect = i == currentTextIndex ? TFT_WHITE : 0x03ff;
        TFTImp::FrameSprite.fillRect(spacer, y, w, h, colorForRect);
        TFTImp::FrameSprite.setTextColor(TFT_BLACK);
        TFTImp::DrawCenteredText(TFTImp::Screen.width() / 2, y + (h / 2), textList[i]);
        TFTImp::FrameSprite.setTextColor(TFT_WHITE);
      }
    }
    else {
      TFTImp::DrawCenteredText("...");
    }
    
    TFTImp::DrawCenteredText(24, TFTImp::Screen.height() - 6, "< Back");
  }

  // GAMES MENU
  void GamesMenu::Init() {
    TextListMenu::Init();

    textList = (const char **)FileImp::GetSubDirectories("/games", &textListCount);
  }
  void GamesMenu::Destroy() {
    TextListMenu::Destroy();
  }
  void GamesMenu::Update(unsigned long dt) {
    TextListMenu::Update(dt);

    if (textListCount != 0 && Input::Buttons[0].justPressed) {
      LuaImp::InitializeGame(textList[currentTextIndex]);

      SetMenu(nullptr);
    }
  }
  void GamesMenu::Draw() {
    TextListMenu::Draw();
  }

  // INSTALL MENU
  void InstallMenu::Init() {
    TextListMenu::Init();

    NetImp::GetGameDownloadList(this);
  }
  void InstallMenu::Destroy() {
    TextListMenu::Destroy();
  }
  void InstallMenu::Update(unsigned long dt) {
    TextListMenu::Update(dt);
    
    if (textListCount != 0 && Input::Buttons[0].justPressed) {
      NetImp::StartGameDownload(currentTextIndex+1);
    }
  }
  void InstallMenu::Draw() {
    TextListMenu::Draw();
  }
  void InstallMenu::DumpDownloadList(char ** downloadList, uint8_t count) {
    textList = (const char **)downloadList;
    textListCount = count;
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