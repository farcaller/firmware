#include "luaTransport.h"
#include "luaLE.h"
#include "MidiInputCallback.h"

int luaopen_transport(lua_State *L)
{
    luaL_newlib(L, transport_functions);
    return 1;
}

int transport_enable(lua_State *L)
{
    lua_settop(L, 0);

    logMessage("transport enabled");

    MidiInputCallback::onMidiClockCallback = &transport_onMidiClock;
    MidiInputCallback::onMidiStartCallback = &transport_onMidiStart;
    MidiInputCallback::onMidiStopCallback = &transport_onMidiStop;
    MidiInputCallback::onMidiContinueCallback = &transport_onMidiContinue;
    MidiInputCallback::onMidiSongSelectCallback = &transport_onMidiSongSelect;
    MidiInputCallback::onMidiSongPositionCallback =
        &transport_onMidiSongPosition;

    return (0);
}

int transport_disable(lua_State *L)
{
    lua_settop(L, 0);

    logMessage("transport disabled");

    MidiInputCallback::onMidiClockCallback = nullptr;
    MidiInputCallback::onMidiStartCallback = nullptr;
    MidiInputCallback::onMidiStopCallback = nullptr;
    MidiInputCallback::onMidiContinueCallback = nullptr;
    MidiInputCallback::onMidiSongSelectCallback = nullptr;
    MidiInputCallback::onMidiSongPositionCallback = nullptr;

    return (0);
}

int transport_isEnabled(lua_State *L)
{
    lua_pushboolean(
        L, (MidiInputCallback::onMidiClockCallback == nullptr) ? false : true);
    return (1);
}

void transport_onMidiClock(MidiInput midiInput)
{
    onSingleByte("onClock", midiInput);
}

void transport_onMidiStart(MidiInput midiInput)
{
    onSingleByte("onStart", midiInput);
}

void transport_onMidiStop(MidiInput midiInput)
{
    onSingleByte("onStop", midiInput);
}

void transport_onMidiContinue(MidiInput midiInput)
{
    onSingleByte("onContinue", midiInput);
}

void transport_onMidiSongSelect(MidiInput midiInput, uint8_t songNumber)
{
    onTwoBytes("onSongSelect", midiInput, songNumber);
}

void transport_onMidiSongPosition(MidiInput midiInput, int position)
{
    onTwoBytes("onSongPosition", midiInput, position);
}

void onSingleByte(const char *function, MidiInput midiInput)
{
    luaLE_getModuleFunction(L, "transport", function);

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableString(
            L,
            "interface",
            MidiInterface::getName(midiInput.getInterfaceType()));
        luaLE_pushTableInteger(L, "portNumber", midiInput.getPort());

        if (lua_pcall(L, 1, 0, 0) != 0) {
            logMessage("lua: error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}

void onTwoBytes(const char *function, MidiInput midiInput, int data1)
{
    luaLE_getModuleFunction(L, "transport", function);

    if (lua_isfunction(L, -1)) {
        logMessage("y");
        lua_newtable(L);
        luaLE_pushTableString(
            L,
            "interface",
            MidiInterface::getName(midiInput.getInterfaceType()));
        luaLE_pushTableInteger(L, "portNumber", midiInput.getPort());
        lua_pushnumber(L, data1);

        if (lua_pcall(L, 2, 0, 0) != 0) {
            logMessage("lua: error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}
