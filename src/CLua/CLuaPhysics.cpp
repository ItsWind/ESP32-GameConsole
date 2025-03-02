#include "src/CLua/CLuaPhysics.h"

struct Vec2 {
  int16_t x;
  int16_t y;
};
struct GamePhysObj {
  bool active;
  Vec2 position;
  Vec2 velocity;
  int16_t w;
  int16_t h;
};
static uint8_t gamePhysObjsIndexInactiveCount = 0;
static uint8_t gamePhysObjsIndexInactive[256];
static uint8_t gamePhysObjsCount = 0;
static GamePhysObj gamePhysObjs[256];

static int luaCreatePhysObj(lua_State * state) {
  if (gamePhysObjsCount >= 255 || lua_gettop(state) != 5 || !lua_istable(state, 1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushboolean(state, 0);
    return 1;
  }

  uint8_t indexToStoreAt = gamePhysObjsCount;
  if (gamePhysObjsIndexInactiveCount > 0) {
    indexToStoreAt = gamePhysObjsIndexInactive[gamePhysObjsIndexInactiveCount - 1];
    gamePhysObjsIndexInactiveCount--;
  }
  
  int16_t x = (int16_t)lua_tonumber(state, 2);
  int16_t y = (int16_t)lua_tonumber(state, 3);
  int16_t w = (int16_t)lua_tonumber(state, 4);
  int16_t h = (int16_t)lua_tonumber(state, 5);

  gamePhysObjs[indexToStoreAt] = {true, {x, y}, {0, 0}, w, h};
  gamePhysObjsCount++;
  
  // width and height in parent collider table
  /*lua_setfield(state, 1, "h");
  lua_setfield(state, 1, "w");

  // Position table at bottom
  lua_newtable(state);
  lua_insert(state, 1);
  lua_setfield(state, 1, "y");
  lua_setfield(state, 1, "x");

  // Velocity table at bottom
  lua_newtable(state);
  lua_insert(state, 1);
  lua_pushnumber(state, 0);
  lua_pushnumber(state, 0);
  lua_setfield(state, 1, "y");
  lua_setfield(state, 1, "x");

  // Put parent collider table at bottom
  lua_insert(state, 1);*/
  
  lua_pushnumber(state, indexToStoreAt);
  lua_setfield(state, 1, "index");

  //lua_setfield(state, 1, "position");
  //lua_setfield(state, 1, "velocity");

  //lua_pop(state, 1);

  lua_pop(state, 5);

  lua_pushboolean(state, 1);
  return 1;
}

static int luaDeletePhysObj(lua_State * state) {
  if (lua_gettop(state) != 1 || !lua_istable(state, 1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushboolean(state, 0);
    return 1;
  }

  lua_getfield(state, 1, "index");
  uint8_t indexToDelete = (uint8_t)lua_tonumber(state, -1);
  if (!gamePhysObjs[indexToDelete].active) {
    lua_pop(state, lua_gettop(state));
    lua_pushboolean(state, 0);
    return 1;
  }

  gamePhysObjs[indexToDelete].active = false;
  gamePhysObjsIndexInactive[gamePhysObjsIndexInactiveCount] = indexToDelete;
  gamePhysObjsIndexInactiveCount++;

  lua_pop(state, 2);

  lua_pushboolean(state, 1);
  return 1;
}

static int luaGetPhysObjVelocity(lua_State * state) {
  if (lua_gettop(state) != 1 || !lua_istable(state, 1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }
  
  lua_getfield(state, 1, "index");
  if (!lua_isnumber(state, -1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }

  uint8_t physObjIndex = (uint8_t)lua_tonumber(state, -1);
  if (!gamePhysObjs[physObjIndex].active) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }

  lua_pushnumber(state, gamePhysObjs[physObjIndex].velocity.x);
  lua_pushnumber(state, gamePhysObjs[physObjIndex].velocity.y);
  lua_remove(state, 1);
  return 2;
}

static int luaGetPhysObjPosition(lua_State * state) {
  if (lua_gettop(state) != 1 || !lua_istable(state, 1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }
  
  lua_getfield(state, 1, "index");
  if (!lua_isnumber(state, -1)) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }

  uint8_t physObjIndex = (uint8_t)lua_tonumber(state, -1);
  if (!gamePhysObjs[physObjIndex].active) {
    lua_pop(state, lua_gettop(state));
    lua_pushnil(state);
    lua_pushnil(state);
    return 2;
  }

  lua_pushnumber(state, gamePhysObjs[physObjIndex].position.x);
  lua_pushnumber(state, gamePhysObjs[physObjIndex].position.y);
  lua_remove(state, 1);
  return 2;
}

namespace CLuaPhysics {
  void Register(lua_State * state) {
    for (int16_t i = 0; i < 256; i++) {
      gamePhysObjs[i] = {false, {0, 0}, {0, 0}, 0, 0};
    }
    gamePhysObjsIndexInactiveCount = 0;
    gamePhysObjsCount = 0;

    lua_register(state, "createPhysObj", luaCreatePhysObj);
    lua_register(state, "deletePhysObj", luaDeletePhysObj);
    lua_register(state, "getPhysObjVelocity", luaGetPhysObjVelocity);
    lua_register(state, "getPhysObjPosition", luaGetPhysObjPosition);
  }
}