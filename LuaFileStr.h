const String LUA_FILE_STR = String(R""""(

print("loading lua file str")

local guyMan = {
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
  --local leftX, leftY = getInputVector(0)
  --print(leftX .. " - " .. leftY)
  --local rightX, rightY = getInputVector(1)
  --print(rightX .. " - " .. rightY)
  print(getInputButtonHeld("BUTTON_CENTER"))
end
guyMan.draw = function()
  drawBox(guyMan.position.x, guyMan.position.y, 5, 5, 255, 255, 255)
end

function init()
  print("Hello from init!")
  tftPrintLn("Hello from init!")
end

function update(dt)
  guyMan.update(dt)
end

function draw()
  guyMan.draw()
end

)"""");