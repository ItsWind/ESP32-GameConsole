#pragma once

#include <TFT_eSPI.h>
#include <SPI.h>

namespace TFTImp {
  extern TFT_eSPI Screen;
  extern TFT_eSprite FrameSprite;

  void Init();
  void PrepareNewFrameSprite();
  void PushCurrentFrameSprite(unsigned long dt);
}