#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace TFTImp {
  extern TFT_eSPI Screen;
  extern TFT_eSprite FrameSprite;

  void Init();
  void PrepareNewFrameSprite();
  void PushCurrentFrameSprite(unsigned long dt);
  void DrawFIMG(int32_t drawX, int32_t drawY, bool flipImgY, uint8_t alphaOffset, const uint8_t * bytes, uint32_t len);
  void DrawBox(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
  void DrawHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
  void DrawVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
  void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text);
  void DrawCenteredText(int16_t x, int16_t y, const char * text);
  void DrawCenteredText(const char * text);
}