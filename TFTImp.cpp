#include "TFTImp.h"

namespace TFTImp {
  TFT_eSPI Screen = TFT_eSPI();
  TFT_eSprite FrameSprite = TFT_eSprite(&Screen);

  void Init() {
    Screen.init();
    Screen.setRotation(45);
    Screen.fillScreen(TFT_BLACK);
    Screen.setTextColor(TFT_WHITE);
    Screen.setTextWrap(false);
    Screen.setCursor(0, 0);

    Screen.println("DISPLAY STARTED");
  }

  void PrepareNewFrameSprite() {
    FrameSprite.deleteSprite();
    FrameSprite = TFT_eSprite(&Screen);
    FrameSprite.createSprite(Screen.width(), Screen.height());
    FrameSprite.fillSprite(TFT_BLACK);
  }

  void PushCurrentFrameSprite(unsigned long dt) {
    FrameSprite.setCursor(0, 0);
    FrameSprite.setTextSize(1);

    char frameFPSBuf[5];
    sprintf(frameFPSBuf, "%.2f", 1000000.0 / dt);

    FrameSprite.print(frameFPSBuf);
    FrameSprite.pushSprite(0, 0);
  }
}