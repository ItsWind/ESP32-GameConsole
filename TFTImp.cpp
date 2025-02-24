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
    //FrameSprite = TFT_eSprite(&Screen);
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

  void DrawBox(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    FrameSprite.fillRect(x, y, w, h, color);
  }

  void DrawHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
    FrameSprite.fillRectHGradient(x, y, w, h, color1, color2);
  }

  void DrawVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2) {
    FrameSprite.fillRectVGradient(x, y, w, h, color1, color2);
  }

  void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text) {
    FrameSprite.setTextSize(size);
    FrameSprite.setCursor(x - (FrameSprite.textWidth(text) / 2), y - (FrameSprite.fontHeight() / 2));
    FrameSprite.print(text);
  }
  void DrawCenteredText(int16_t x, int16_t y, const char * text) {
    DrawCenteredText(1, x, y, text);
  }
  void DrawCenteredText(const char * text) {
    DrawCenteredText(Screen.width() / 2, Screen.height() / 2, text);
  }
}