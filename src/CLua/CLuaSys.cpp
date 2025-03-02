#include "src/CLua/CLuaSys.h"
#include "src/Imps/MenuImp.h"
#include "src/Imps/FileImp.h"
#include "src/Imps/LuaImp.h"

static int luaCloseGame(lua_State * state) {
  LuaImp::CloseGame();
  MenuImp::SetMenu(new MenuImp::MainMenu());
  return 0;
}

static int luaPrint(lua_State * state) {
  int n = lua_gettop(state);  /* number of arguments */
  int i;
  lua_getglobal(state, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    size_t l;
    lua_pushvalue(state, -1);  /* function to be called */
    lua_pushvalue(state, i);   /* value to print */
    lua_call(state, 1, 1);
    s = lua_tolstring(state, -1, &l);  /* get result */
    if (s == NULL)
      return luaL_error(state, "'tostring' must return a string to 'print'");
    if (i>1) Serial.write("\t");
    Serial.write(s);
    lua_pop(state, 1);  /* pop result */
  }
  Serial.println();
  return 0;
}

static int luaRequire(lua_State * state) {
  // Get string name passed
  const char * reqStrName = lua_tostring(state, 1);

  // Check package.loaded for key entry of reqStrName
  lua_getglobal(state, "package");  // Push 'package' onto the stack
  lua_getfield(state, -1, "loaded"); // Push 'package.loaded' onto the stack
  lua_remove(state, -2);  // Remove 'package' from the stack, leaving 'package.loaded'
  lua_pushstring(state, reqStrName);
  lua_gettable(state, -2);

  // If found in loaded, then return table found
  if (lua_istable(state, -1)) {
    lua_remove(state, -2); // Remove package.loaded
    lua_remove(state, -2); // Remove reqStrName
    return 1;
  }
  // If not found, pop and continue
  lua_pop(state, 1);

  // Check package.preload for key entry of reqStrName
  lua_getglobal(state, "package");  // Push 'package' onto the stack
  lua_getfield(state, -1, "preload"); // Push 'package.preload' onto the stack
  lua_remove(state, -2);  // Remove 'package' from the stack, leaving 'package.preload'
  lua_pushstring(state, reqStrName);
  lua_gettable(state, -2);
  lua_remove(state, -2); // remove preload table, currently: [reqStrName, package.loaded, package.preload.reqStrName]

  if (lua_isfunction(state, -1)) {
    // Execute function if function found
    if (lua_pcall(state, 0, 1, 0) != LUA_OK) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0; // Function execution failed
    }

    // current stack: [reqStrName, package.loaded, returnTable]
    
    int returnsCount = lua_gettop(state) - 2;
    // Check if function executed returned a value
    if (returnsCount != 1) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0;
    }
    // Check if first value is not a table
    else if (!lua_istable(state, 3)) {
      lua_pop(state, lua_gettop(state)); // pop all
      return 0;
    }

    // Put new table into loaded
    lua_pushstring(state, reqStrName); // [reqStrName, package.loaded, returnTable, reqStrName]
    lua_insert(state, -2); // [reqStrName, package.loaded, reqStrName, returnTable]
    lua_settable(state, -3); // pops reqStrName at top and returnTable, so we have to get return table back
    // [reqStrName, package.loaded]
    lua_pushstring(state, reqStrName);
    lua_gettable(state, -2);
    lua_remove(state, -2); // remove package.loaded
    lua_remove(state, -2); // remove reqStrName
    
    return 1;
  }
  lua_pop(state, 1); // pop package.preload.reqStrName [reqStrName, package.loaded]
  lua_pop(state, 1); // pop package.loaded [reqStrName]

  // Check file system for file require
  String filePathForRequire = "/games/" + String(LuaImp::CurrentGameDirName) + "/" + String(reqStrName) + ".lua";
  const char * fileData = FileImp::GetFileData(filePathForRequire.c_str(), nullptr);
  if (fileData != nullptr) {
    String loadFileStr = "package.preload[\"" + String(reqStrName) + "\"] = function() " + String(fileData) + " end";
    luaL_dostring(state, loadFileStr.c_str());

    delete[] fileData;

    // Redo require with package.preload.reqStrName loaded
    return luaRequire(state);
  }

  // Nothing found, pop all and return nothing
  lua_pop(state, lua_gettop(state));
  
  return 0;
}

namespace CLuaSys {
  void Register(lua_State * state) {
    lua_register(state, "closeGame", luaCloseGame);
    lua_register(state, "print", luaPrint);
    lua_register(state, "require", luaRequire);
  }
}