#include "TFTImp.h"

struct PaletteColor {
  uint16_t rgb565;
  uint8_t alpha;
};

static uint16_t blendRGB565(uint16_t fgColor, uint16_t bgColor, uint8_t alpha) {
    // Extract RGB components from the foreground color (fgColor)
    uint8_t fgR = (fgColor >> 11) & 0x1F; // Extract red (5 bits)
    uint8_t fgG = (fgColor >> 5) & 0x3F;  // Extract green (6 bits)
    uint8_t fgB = fgColor & 0x1F;         // Extract blue (5 bits)

    // Extract RGB components from the background color (bgColor)
    uint8_t bgR = (bgColor >> 11) & 0x1F; // Extract red (5 bits)
    uint8_t bgG = (bgColor >> 5) & 0x3F;  // Extract green (6 bits)
    uint8_t bgB = bgColor & 0x1F;         // Extract blue (5 bits)

    // Normalize alpha value from 0-255 to 0.0-1.0
    float alphaNorm = alpha / 255.0f;
    
    // Blend each color component
    uint8_t outR = (uint8_t)((fgR * alphaNorm) + (bgR * (1.0f - alphaNorm)));
    uint8_t outG = (uint8_t)((fgG * alphaNorm) + (bgG * (1.0f - alphaNorm)));
    uint8_t outB = (uint8_t)((fgB * alphaNorm) + (bgB * (1.0f - alphaNorm)));

    // Clamp the blended values to fit their respective bit widths (5 bits for R/B, 6 bits for G)
    outR = outR > 31 ? 31 : outR;
    outG = outG > 63 ? 63 : outG;
    outB = outB > 31 ? 31 : outB;

    // Reassemble the blended color back into RGB565 format
    uint16_t blendedColor = (outR << 11) | (outG << 5) | outB;

    return blendedColor;
}

static void doFIMGPixel(PaletteColor palColor, int32_t drawX, int32_t drawY, bool flipImgY, uint16_t imgWidth, uint16_t * imgWidthProcessed, uint16_t * imgWidthRow) {
  if (palColor.alpha > 0) {
    int32_t imgWidthProcessedFlipCheck = flipImgY ? (int32_t)imgWidth - (int32_t)*imgWidthProcessed : (int32_t)*imgWidthProcessed;
    int32_t pixelX = imgWidthProcessedFlipCheck + drawX;
    int32_t pixelY = (int32_t)*imgWidthRow + drawY;
    uint16_t colorToDraw = palColor.rgb565;
    
    if (palColor.alpha < 255) {
      colorToDraw = blendRGB565(colorToDraw, TFTImp::FrameSprite.readPixel(pixelX, pixelY), palColor.alpha);
    }

    TFTImp::FrameSprite.drawPixel(pixelX, pixelY, colorToDraw);
  }

  *imgWidthProcessed += 1;
  if (*imgWidthProcessed >= imgWidth) {
    *imgWidthProcessed = 0;
    *imgWidthRow += 1;
  }
}

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
  }

  void PrepareNewFrameSprite() {
    FrameSprite.deleteSprite();
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

  void DrawFIMG(int32_t drawX, int32_t drawY, bool flipImgY, uint8_t alphaOffset, const uint8_t * bytes, uint32_t len) {
    uint16_t imgWidth = (bytes[0] << 8) | bytes[1];
    uint16_t imgWidthProcessed = 0;
    uint16_t imgWidthRow = 0;

    PaletteColor palette[255];
    uint8_t colorsInPalette = bytes[2];
    uint8_t colorsInPaletteProcessed = 0;

    // Loop through bytes
    int currentIndex = 3;
    while (currentIndex < len) {
      // Process palettes to store
      if (colorsInPaletteProcessed < colorsInPalette) {
        int16_t alphaWithOffsetApplied = (int16_t)bytes[currentIndex+2] - (int16_t)alphaOffset;
        alphaWithOffsetApplied = constrain(alphaWithOffsetApplied, 0, 255);//alphaWithOffsetApplied > 255 ? 255 : alphaWithOffsetApplied < 0 ? 0 : alphaWithOffsetApplied;

        palette[colorsInPaletteProcessed] = {(bytes[currentIndex] << 8) | bytes[currentIndex+1], (uint8_t)alphaWithOffsetApplied};
        colorsInPaletteProcessed++;
        currentIndex += 3;
      }
      // Read pixels
      else {
        // 255 = condense code
        if (bytes[currentIndex] == 255) {
          for (uint8_t i = 0; i < bytes[currentIndex+2]; i++) {
            doFIMGPixel(palette[bytes[currentIndex+1]], drawX, drawY, flipImgY, imgWidth, &imgWidthProcessed, &imgWidthRow);
          }
          currentIndex += 3;
        }
        // Not condensed, read palette index
        else {
          doFIMGPixel(palette[bytes[currentIndex]], drawX, drawY, flipImgY, imgWidth, &imgWidthProcessed, &imgWidthRow);
          currentIndex++;
        }
      }
    }
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