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

const String luaFileStr = String(R""""(
print("loading lua file str")
function init()
  print("Hello!")
end
)"""");

void setup() {
  Serial.begin(115200);

  Serial.println("Hello?");
  Serial.print(lua.Lua_dostring(&luaFileStr));
}

void loop() {
  // put your main code here, to run repeatedly:

}
