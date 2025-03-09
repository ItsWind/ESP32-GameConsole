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
    int16_t halfWidth = TFTImp::Screen.width() / 2;
    int16_t halfHeight = TFTImp::Screen.height() / 2;

    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText(halfWidth, halfHeight - 16, "^");
    TFTImp::DrawCenteredText(halfWidth, halfHeight - 24, "Install");
    TFTImp::DrawCenteredText(halfWidth, halfHeight + 16, "v");
    TFTImp::DrawCenteredText(halfWidth, halfHeight + 24, "Play");
  }

  // SERIAL UPLOAD MENU
  void SerialUploadMenu::Init() {
    incomingPacketByteCount = -1;
    incomingPacketIndex = 0;
    incomingPacket = nullptr;
  }
  void SerialUploadMenu::Destroy() {
    if (incomingPacket != nullptr) {
      delete[] incomingPacket;
    }
  }
  void SerialUploadMenu::Update(unsigned long dt) {
    if (!NetImp::DownloadingGame && Input::Buttons[4].justPressed) {
      SetMenu(new InstallMenu());
      return;
    }

    if (Serial.available() <= 0) {
      return;
    }

    if (incomingPacketByteCount < 0) {
      incomingPacketByteCount = (int16_t)Serial.read();
      Serial.println("Incoming packet count received");
    }
    else {
      if (incomingPacket == nullptr) {
        Serial.println("Incoming packet created");
        incomingPacket = new uint8_t[incomingPacketByteCount];
      }
      
      incomingPacket[incomingPacketIndex] = Serial.read();
      Serial.println("bong");
      
      if ((int16_t)incomingPacketIndex >= incomingPacketByteCount - 1) {
        NetImp::ProcessPacket((const uint8_t *)incomingPacket, (size_t)incomingPacketByteCount, true);
        Serial.println("nyoomed");

        delete[] incomingPacket;
        incomingPacketByteCount = -1;
        incomingPacketIndex = 0;
        incomingPacket = nullptr;
      }
      else {
        incomingPacketIndex++;
      }
    }
  }
  void SerialUploadMenu::Draw() {
    TFTImp::DrawCenteredText("Serial");
    TFTImp::DrawCenteredText(24, TFTImp::Screen.height() - 6, "< Back");

    /*int16_t halfWidth = TFTImp::Screen.width() / 2;
    int16_t halfHeight = TFTImp::Screen.height() / 2;

    TFTImp::FrameSprite.fillSprite(TFT_BLUE);
    TFTImp::FrameSprite.setTextColor(TFT_WHITE);
    TFTImp::DrawCenteredText(halfWidth, halfHeight - 16, "▲");*/
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
      return;
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

    resendGetListTimer = 0;
    textList = nullptr;
    NetImp::GetGameDownloadList(this);
  }
  void InstallMenu::Destroy() {
    TextListMenu::Destroy();
    NetImp::CancelGameDownloadList();
  }
  void InstallMenu::Update(unsigned long dt) {
    if (NetImp::DownloadingGame) {
      return;
    }
    TextListMenu::Update(dt);

    if (Input::Buttons[2].justPressed) {
      SetMenu(new SerialUploadMenu());
      return;
    }

    // Resend packet if textList never assigned to
    if (textList == nullptr) {
      resendGetListTimer += dt;
      if (resendGetListTimer >= 1000000 && textList == nullptr) {
        resendGetListTimer = 0;
        NetImp::GetGameDownloadList(this);
      }
    }
    
    if (textListCount != 0 && Input::Buttons[0].justPressed) {
      NetImp::StartGameDownload(currentTextIndex+1);
    }
  }
  void InstallMenu::Draw() {
    TextListMenu::Draw();

    TFTImp::DrawCenteredText(TFTImp::Screen.width() - 24, TFTImp::Screen.height() - 6, "Serial >");
  }
  void InstallMenu::DumpDownloadList(char ** downloadList, uint8_t count) {
    // If textList is already populated
    if (textList != nullptr) {
      for (uint8_t i = 0; i < count; i++) {
        delete[] downloadList[i];
      }
      delete[] downloadList;
      return;
    }

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