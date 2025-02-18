#include "MenuImp.h"
#include "Input.h"
#include "TFTImp.h"
#include "LuaImp.h"

namespace MenuImp {
  Menu MenuList[1] = {
    {
      [](unsigned long dt) {
        if (Input::Buttons[0].justPressed) {
          LuaImp::InitializeGame();

          SetMenuTo(-1);
        }
      },
      []() {
        TFTImp::FrameSprite.fillSprite(TFT_BLUE);
        TFTImp::FrameSprite.setTextColor(TFT_WHITE);
        TFTImp::DrawCenteredText(TFTImp::Screen.width() / 2, TFTImp::Screen.height() / 2, "Hello!");
      }
    }
  };
  Menu * CurrentMenu = &MenuList[0];

  void SetMenuTo(int8_t index) {
    if (index < 0) {
      CurrentMenu = nullptr; 
    }
    else {
      CurrentMenu = &MenuList[index];
    }
  }
}