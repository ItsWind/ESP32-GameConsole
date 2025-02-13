#include "LuaFileStr.h"

const char * LUA_FILE_STR = R""""(

print("loading lua file str")
--math.randomseed(RANDOMSEEDNUM)

local function getNormalized(x, y)
  local vectorLen = math.sqrt(x * x + y * y)
  
  if vectorLen == 0 then
    return 0, 0
  end

  return x / vectorLen, y / vectorLen
end

local function lerp(a, b, t)
	return a + (b - a) * t
end

local guyMan = {
  speed = 128,
  velocity = {
    x = 0,
    y = 0
  },
  position = {
    x = 30,
    y = 30
  }
}
guyMan.update = function(dt)
  local leftX, leftY = getInputVector(0)
  local normX, normY = getNormalized(leftX, leftY)

  local speedMult = 1
  if getInputButtonHeld("BUTTON_JOYSTICK_RIGHT") then
    speedMult = 4
  end

  -- velocity X input
  if normX ~= 0 then
    guyMan.velocity.x = lerp(guyMan.velocity.x, normX * guyMan.speed * speedMult, dt)
  end

  -- velocity Y input
  if normY ~= 0 then
    guyMan.velocity.y = lerp(guyMan.velocity.y, normY * guyMan.speed * speedMult, dt)
  end

  -- Apply velocity to position
  guyMan.position.x = guyMan.position.x + (guyMan.velocity.x * dt)
  guyMan.position.y = guyMan.position.y + (guyMan.velocity.y * dt)

  -- velocity X damping
  if guyMan.velocity.x ~= 0 then
    guyMan.velocity.x = guyMan.velocity.x * 0.1^dt
    if math.abs(guyMan.velocity.x) < 0.01 then guyMan.velocity.x = 0 end
  end

  -- velocity Y damping
  if guyMan.velocity.y ~= 0 then
    guyMan.velocity.y = guyMan.velocity.y * 0.1^dt
    if math.abs(guyMan.velocity.y) < 0.01 then guyMan.velocity.y = 0 end
  end
end
guyMan.draw = function()
  drawBox(guyMan.position.x, guyMan.position.y, 5, 5, 255, 255, 255)
end

function init()
  print("Hello from init!")
  tftPrint("Hello from init!")
  print(math.random(100))
end

function update(dt)
  guyMan.update(dt)
end

function draw()
  guyMan.draw()
end

)"""";