#include "Util.h"

namespace Util {
  uint16_t rgb888_to_rgb565(uint8_t R, uint8_t G, uint8_t B) {
    uint16_t r_565 = (R >> 3) & 0x1F; // Red component
    uint16_t g_565 = (G >> 2) & 0x3F; // Green component
    uint16_t b_565 = (B >> 3) & 0x1F; // Blue component
    
    return (r_565 << 11) | (g_565 << 5) | b_565;
  }

  void rgb565_to_rgb888(uint16_t rgb565, uint8_t* R, uint8_t* G, uint8_t* B) {
    // Extract the red, green, and blue components from the RGB565 value
    uint16_t r_565 = (rgb565 >> 11) & 0x1F;  // Extract the 5 bits for red
    uint16_t g_565 = (rgb565 >> 5) & 0x3F;   // Extract the 6 bits for green
    uint16_t b_565 = rgb565 & 0x1F;          // Extract the 5 bits for blue

    // Convert each component to 8 bits
    *R = (r_565 << 3) | (r_565 >> 2);  // Red is expanded from 5 to 8 bits
    *G = (g_565 << 2) | (g_565 >> 4);  // Green is expanded from 6 to 8 bits
    *B = (b_565 << 3) | (b_565 >> 2);  // Blue is expanded from 5 to 8 bits
  }
}