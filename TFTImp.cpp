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

    char frameFPSBuf[6];
    sprintf(frameFPSBuf, "%.2f", 1000000.0 / dt);

    FrameSprite.print(frameFPSBuf);
    FrameSprite.pushSprite(0, 0);
  }

  void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text) {
    int16_t x1;
    int16_t y1;
    uint16_t w;
    uint16_t h;
  
    FrameSprite.setTextSize(size);
  
    //tft.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  
    FrameSprite.setCursor(x - (FrameSprite.textWidth(text) / 2), y - (FrameSprite.fontHeight() / 2));
  
    //tft.setCursor(x1 - (w / 2), y1 - (h / 2));
    FrameSprite.print(text);
  }

  void DrawCenteredText(int16_t x, int16_t y, const char * text) {
    DrawCenteredText(1, x, y, text);
  }
}