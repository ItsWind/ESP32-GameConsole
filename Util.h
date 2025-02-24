#pragma once

#include <Arduino.h>

namespace Util {
  uint16_t rgb888_to_rgb565(uint8_t R, uint8_t G, uint8_t B);
  void rgb565_to_rgb888(uint16_t rgb565, uint8_t * R, uint8_t * G, uint8_t * B);
}