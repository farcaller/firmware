#include "luaLE.h"
#include "helpers.h"
#include "SysexBlock.h"

void luaLE_openEoslibs(lua_State *L, const luaL_Reg *libs)
{
    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = libs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1); /* remove lib */
    }
}

void luaLE_pushTableString(lua_State *L, const char *key, const char *value)
{
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
}

void luaLE_pushTableInteger(lua_State *L, const char *key, int value)
{
    lua_pushstring(L, key);
    lua_pushinteger(L, value);
    lua_settable(L, -3);
}

void luaLE_pushArrayInteger(lua_State *L, int key, int value)
{
    lua_pushinteger(L, key);
    lua_pushinteger(L, value);
    lua_settable(L, -3);
}

void luaLE_pushArrayString(lua_State *L, int key, const char *value)
{
    lua_pushinteger(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
}

void luaLE_pushArrayObject(lua_State *L,
                           int key,
                           const char *objectName,
                           void *object)
{
    lua_pushinteger(L, key);
    luaLE_pushObject(L, objectName, object);
    lua_settable(L, -3);
}

void luaLE_pushObject(lua_State *L, const char *objectName, void *object)
{
    *reinterpret_cast<void **>(lua_newuserdata(L, sizeof(void *))) = object;

    luaL_getmetatable(L, objectName);
    lua_setmetatable(L, -2);
}

void luaLE_getModuleFunction(lua_State *L,
                             const char *module,
                             const char *function)
{
    lua_getglobal(L, module);
    lua_pushstring(L, function);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}

void luaLE_handleNonexistentFunction(lua_State *L, const char *function)
{
    lua_pop(L, 1);
    logMessage("lua: function %s does not exist", function);
}

void luaLE_addObjectMethods(lua_State *L, const luaL_Reg *l)
{
    for (; l->name; l++) {
        lua_pushcfunction(L, l->func);
        lua_setfield(L, -(2), l->name);
    }
}

void luaLE_dumpstack(lua_State *L)
{
    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++) {
        logMessage("%d\t%s   ", i, luaL_typename(L, i));

        switch (lua_type(L, i)) {
            case LUA_TNUMBER:
                logMessage("%g\n", lua_tonumber(L, i));
                break;
            case LUA_TSTRING:
                logMessage("%s\n", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                logMessage("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
                break;
            case LUA_TNIL:
                logMessage("%s\n", "nil");
                break;
            default:
                logMessage("%p\n", lua_topointer(L, i));
                break;
        }
    }
}
