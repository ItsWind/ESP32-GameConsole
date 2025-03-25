#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace TFTImp {
  extern TFT_eSPI Screen;
  extern TFT_eSprite FrameSprite;

  void Init();
  void SetScreenDim(uint8_t dimPwm);
  uint8_t GetScreenDim();
  void PrepareNewFrameSprite();
  void PushCurrentFrameSprite(unsigned long dt);
  void DrawFIMG(int32_t drawX, int32_t drawY, bool flipImgY, uint8_t alphaOffset, const uint8_t * bytes, uint32_t len);
  void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text);
  void DrawCenteredText(int16_t x, int16_t y, const char * text);
  void DrawCenteredText(const char * text);
}