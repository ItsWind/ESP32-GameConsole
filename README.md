# ESP32-GameConsole
 A handheld gaming console project for ESP32

Current plans:
- Lua as the main programming language for game code.
- C functions as the backend to communicate with ESP32, which communicates with screen and other peripherals.
- 2D graphics only. 3D may be a bit much for the ESP32, but maybe for a far far future goal.
- Built-in physics and graphics lib that can be interacted with through Lua.
- Built-in UDP/TCP basic functionalities for multiplayer. May also consider making a built-in multiplayer library for ease of use.
- Open source schematics (and code ofc) so that anyone can build their own.
- Website upload of Lua code to direct download games.
- External storage (reading game code from SD card) could pose threat for multiplayer games. May allow serial transfer of data.
