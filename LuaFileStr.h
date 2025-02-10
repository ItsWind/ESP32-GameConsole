const String LUA_FILE_STR = String(R""""(

print("loading lua file str")

local guyMan = {
  x = 30,
  y = 30,
  w = 5,
  h = 5
}
guyMan.draw = function()
  drawBox(guyMan.x, guyMan.y, guyMan.w, guyMan.h, 255, 255, 255)
end

function init()
  print("Hello from init!")
  tftPrintLn("Hello from init!")
end

function update(dt)
  --
end

function draw()
  guyMan.draw()
end

function input(name, type, value, value2)
  --print(name .. " : " .. type .. " : " .. value .. " : " .. value2)
  if name == "JOYSTICK_LEFT" then
    guyMan.x = guyMan.x + value
    guyMan.y = guyMan.y - value2
  end
end

)"""");