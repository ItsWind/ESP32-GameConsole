#include "src/CLua/CLuaDraw.h"
#include "Util.h"
#include "src/Imps/MenuImp.h"
#include "src/Imps/FileImp.h"
#include "src/Imps/TFTImp.h"
#include "src/Imps/LuaImp.h"

struct FIMGCached {
  const char * name;
  const uint8_t * data;
  uint32_t dataLen;
};
static uint8_t fimgCachedAmount = 0;
static FIMGCached fimgCache[256];

static int luaTFTPrint(lua_State * state) {
  const char * str = lua_tostring(state, 1);

  TFTImp::Screen.println(str);

  lua_pop(state, 1);

  return 0;
}

static int luaDrawFIMG(lua_State * state) {
  const char * filePath = lua_tostring(state, 1);
  int32_t x = (int32_t)lua_tonumber(state, 2);
  int32_t y = (int32_t)lua_tonumber(state, 3);
  bool flipImgY = (bool)lua_toboolean(state, 4);
  uint8_t alphaOffset = (uint8_t)lua_tonumber(state, 5);

  int16_t cachedIndex = -1;
  for (int16_t i = 0; i < fimgCachedAmount; i++) {
    if (strcmp(fimgCache[i].name, filePath) == 0) {
      cachedIndex = i;
      break;
    }
  }

  uint32_t fileDataLen = 0;
  const uint8_t * filePathData = nullptr;

  if (cachedIndex == -1) {
    if (fimgCachedAmount >= 255) {
      LuaImp::CloseGame();
      MenuImp::SetMenu(new MenuImp::MessageMenu("FIMG cache overflow."));
      return 0;
    }
    String fullPath = "/games/" + String(LuaImp::CurrentGameDirName) + "/" + String(filePath) + ".fimg";
    filePathData = (const uint8_t *)FileImp::GetFileData(fullPath.c_str(), &fileDataLen);

    if (filePathData != nullptr) {
      size_t filePathStrLen = strlen(filePath);
      char * filePathCopy = new char[filePathStrLen+1];
      strcpy(filePathCopy, filePath);
      filePathCopy[filePathStrLen] = '\0';

      fimgCache[fimgCachedAmount] = {(const char *)filePathCopy, filePathData, fileDataLen};
      fimgCachedAmount++;
    }
  }
  else {
    fileDataLen = fimgCache[cachedIndex].dataLen;
    filePathData = fimgCache[cachedIndex].data;
  }

  if (filePathData != nullptr) {
    TFTImp::DrawFIMG(x, y, flipImgY, alphaOffset, filePathData, fileDataLen);
  }

  lua_pop(state, 5);

  return 0;
}

static int luaDrawBox(lua_State * state) {//(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b) {
  int32_t x = (int32_t)lua_tonumber(state, 1);
  int32_t y = (int32_t)lua_tonumber(state, 2);
  int32_t w = (int32_t)lua_tonumber(state, 3);
  int32_t h = (int32_t)lua_tonumber(state, 4);
  uint8_t r = (uint8_t)lua_tonumber(state, 5);
  uint8_t g = (uint8_t)lua_tonumber(state, 6);
  uint8_t b = (uint8_t)lua_tonumber(state, 7);

  TFTImp::DrawBox(x, y, w, h, Util::rgb888_to_rgb565(r, g, b));

  lua_pop(state, 7);

  return 0;
}

namespace CLuaDraw {
  void Register(lua_State * state) {
    lua_register(state, "tftPrint", luaTFTPrint);
    lua_register(state, "drawFIMG", luaDrawFIMG);
    lua_register(state, "drawBox", luaDrawBox);
  }

  void Close() {
    for (int16_t i = 0; i < fimgCachedAmount; i++) {
      delete[] fimgCache[i].name;
      delete[] fimgCache[i].data;
    }
    fimgCachedAmount = 0;
  }
}