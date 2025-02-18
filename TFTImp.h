#pragma once

#include <TFT_eSPI.h>
#include <SPI.h>

namespace TFTImp {
  extern TFT_eSPI Screen;
  extern TFT_eSprite FrameSprite;

  void Init();
  void PrepareNewFrameSprite();
  void PushCurrentFrameSprite(unsigned long dt);
  void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text);
  void DrawCenteredText(int16_t x, int16_t y, const char * text);
}