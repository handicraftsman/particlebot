#pragma once

#ifdef PB_PLUGIN
  #define private public
  #define protected public
#endif

#include "EventMachine.hpp"
#include "Event.hpp"
#include "User.hpp"
#include "PluginManager.hpp"
#include "Plugin.hpp"
#include "CommandInfo.hpp"
#include "Bot.hpp"
#include "IRCSocket.hpp"