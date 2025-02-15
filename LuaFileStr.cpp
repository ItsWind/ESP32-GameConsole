#include "LuaFileStr.h"

const char * LUA_FILE_STR = R""""(

print("loading lua file str")

local Collision = require("SimpleCollisions")

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
  }
}
guyMan.init = function()
  Collision.AddBox(guyMan, 30, 30, 5, 5)
end
guyMan.update = function(dt)
  -- Apply velocity to position
  Collision.SlideBox(guyMan, guyMan.velocity.x * dt, guyMan.velocity.y * dt)
end
guyMan.fixedupdate = function(dt)
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
  drawBox(guyMan.box.x, guyMan.box.y, guyMan.box.w, guyMan.box.h, 255, 255, 255)
end

local walls = {
  {
    x = 60,
    y = 60,
    w = 30,
    h = 10
  },
  {
    x = 30,
    y = 70,
    w = 10,
    h = 20
  }
}

function Init()
  print("Hello from init!")
  tftPrint("Hello from init!")
  print(math.random(100))

  guyMan.init()

  for i=1, #walls do
    Collision.AddBox(walls[i], walls[i].x, walls[i].y, walls[i].w, walls[i].h)
  end
end

local xMultDebug = 1
function Update(dt)
  guyMan.update(dt)

  if walls[1].box.x > 90 then
    xMultDebug = -1
  elseif walls[1].box.x < 20 then
    xMultDebug = 1
  end

  Collision.SlideBox(walls[1], 64 * dt * xMultDebug, 0)
end

function FixedUpdate(dt)
  guyMan.fixedupdate(dt)
end

function Draw()
  for i=1, #walls do
    drawBox(walls[i].box.x, walls[i].box.y, walls[i].box.w, walls[i].box.h, 255, 0, 0)
  end

  guyMan.draw()
end

)"""";