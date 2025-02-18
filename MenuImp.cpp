#include "MenuImp.h"

namespace MenuImp {
  Menu MenuList[1] = {
    {
      /*[dt] {
        // Update
      },*/
      [] {
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