#include <LuaWrapper.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

LuaWrapper lua;
TFT_eSPI tft = TFT_eSPI();

const String luaFileStr = String(R""""(
print("loading lua file str")

function init()
  print("Hello from init!")
  tftPrintLn("Hello from init!")
end

function update(dt)
  --print("Hello!")
end

function draw()
  --print("Hello!")
end
)"""");

int luaDoTFTPrintLn(lua_State * state) {
  const char * str = lua_tostring(state, 1);

  if (str != NULL) {
    tft.println(str);
  }

  lua_pop(state, 1);

  return 0;
}

// Time variables for loop (SET AT END OF setup)
unsigned long oldTime = 0;
void setup() {
  Serial.begin(115200);

  lua_register(lua.State, "tftPrintLn", luaDoTFTPrintLn);

  Serial.println("Hello?");
  Serial.print(lua.Lua_dostring(&luaFileStr));

  tft.init();
  tft.setRotation(45);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setCursor(0, 0);

  tft.println("DISPLAY STARTED");

  lua_getglobal(lua.State, "init");
  if (lua_pcall(lua.State, 0, 0, 0) != LUA_OK) {
    Serial.println("error calling init");
  }

  // SET LOOP TIME AT END OF setup
  oldTime = micros();
}

void loop() {
  unsigned long thisTime = micros();
  unsigned long dt = 0;
  // Handle overflow
  if (thisTime < oldTime) {
    unsigned long maxNum = 0;
    maxNum--;

    dt = (maxNum - oldTime) + thisTime + 1;
  }
  else {
    dt = thisTime - oldTime;
  }
  oldTime = thisTime;


}
