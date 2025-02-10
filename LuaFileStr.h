const String LUA_FILE_STR = String(R""""(

print("loading lua file str")

function init()
  print("Hello from init!")
  tftPrintLn("Hello from init!")
end

function update(dt)
  --print(dt)
end

function input(name, value)
  print(name .. ": " .. value)
end

)"""");