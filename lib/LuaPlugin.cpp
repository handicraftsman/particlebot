#define PB_PLUGIN

#include "include.hpp"

static std::string find_path_to_plugin(std::string name) {
  const std::vector<std::string> paths {
    "./pb-" + name + ".lua",
    "./pb-" + name + "/main.lua",
    "../pb-" + name + ".lua",
    "../pb-" + name + "/main.lua",
    "/usr/share/pb-" + name + ".lua",
    "/usr/share/pb-" + name + "/main.lua",
    "/usr/local/share/pb-" + name + ".lua",
    "/usr/local/share/pb-" + name + "/main.lua"
  };

  for (std::string path : paths) {
    if (std::ifstream(path).good())
      return path;
  }

  throw std::runtime_error("Unable to find lua plugin " + name);
}

static PB::LuaPlugin* get_self(lua_State* L) {
  lua_getglobal(L, "__pb_addr");
  void** s = (void**) lua_touserdata(L, -1);
  lua_pop(L, 1);
  return (PB::LuaPlugin*) *s; 
}

static std::map<PB::EventType, std::string> names {
  { PB::EventType::Event,           "Event"           },
  { PB::EventType::TestEvent,       "TestEvent"       },
  { PB::EventType::ConnectEvent,    "ConnectEvent"    },
  { PB::EventType::DisconnectEvent, "DisconnectEvent" },
  { PB::EventType::IRCMessageEvent, "IRCMessageEvent" },
  { PB::EventType::CodeEvent,       "CodeEvent"       },
  { PB::EventType::PingEvent,       "PingEvent"       },
  { PB::EventType::WHOREPLYEvent,   "WHOREPLYEvent"   },
  { PB::EventType::JoinEvent,       "JoinEvent"       },
  { PB::EventType::PartEvent,       "PartEvent"       },
  { PB::EventType::NickEvent,       "NickEvent"       },
  { PB::EventType::PRIVMSGEvent,    "PRIVMSGEvent"    },
  { PB::EventType::PRIVMSGEvent,    "CTCPEvent"       },
  { PB::EventType::NOTICEEvent,     "NOTICEEvent"     },
  { PB::EventType::CommandEvent,    "CommandEvent"    },
};

static bool get_table_value(lua_State* L, std::string type) {
  lua_pushnil(L);

  while (lua_next(L, -2)) {
    if (lua_isstring(L, -2) && type == std::string(lua_tostring(L, -2)))
      return true;
    lua_pop(L, 1);
  }

  lua_pop(L, 1);

  return false;
}

void stackdump_g(lua_State* l) {
  int i;
  int top = lua_gettop(l);

  printf("total in stack %d\n",top);
 
  for (i = 1; i <= top; i++) {
    int t = lua_type(l, i);
    printf("  ");
    switch (t) {
    case LUA_TSTRING:
      printf("string: '%s'\n", lua_tostring(l, i));
      break;

    case LUA_TBOOLEAN:
      printf("boolean %s\n",lua_toboolean(l, i) ? "true" : "false");
      break;

    case LUA_TNUMBER:
      printf("number: %g\n", lua_tonumber(l, i));
      break;

    default:
      printf("%s\n", lua_typename(l, t));
      break;
    }
  }
  printf("\n");
}

static void push_event_table(lua_State* L, PB::Event* _e) {
  switch (_e->type()) {
  case PB::EventType::TestEvent: {
    lua_createtable(L, 0, 1);
    lua_pushstring(L, ((PB::TestEvent*) _e)->str.c_str());
    lua_setfield(L, -2, "str");
    break;
  }

  case PB::EventType::ConnectEvent: {
    lua_createtable(L, 0, 1);
    lua_pushstring(L, ((PB::ConnectEvent*) _e)->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    break;
  }

  case PB::EventType::DisconnectEvent: {
    lua_createtable(L, 0, 1);
    lua_pushstring(L, ((PB::DisconnectEvent*) _e)->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    break;
  }

  case PB::EventType::IRCMessageEvent: {
    PB::IRCMessageEvent* e = (PB::IRCMessageEvent*) _e;
    lua_createtable(L, 0, 2);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->raw_msg.c_str());
    lua_setfield(L, -2, "raw_msg");
    break;
  }

  case PB::EventType::CodeEvent: {
    PB::CodeEvent* e = (PB::CodeEvent*) _e;
    lua_createtable(L, 0, 3);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->code.c_str());
    lua_setfield(L, -2, "code");
    lua_pushstring(L, e->extra.c_str());
    lua_setfield(L, -2, "extra");
    break;
  }

  case PB::EventType::PingEvent: {
    PB::PingEvent* e = (PB::PingEvent*) _e;
    lua_createtable(L, 0, 2);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->target.c_str());
    lua_setfield(L, -2, "target");
    break;
  }

  case PB::EventType::WHOREPLYEvent: {
    PB::WHOREPLYEvent* e = (PB::WHOREPLYEvent*) _e;
    lua_createtable(L, 0, 4);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    break;
  }

  case PB::EventType::JoinEvent: {
    PB::JoinEvent* e = (PB::JoinEvent*) _e;
    lua_createtable(L, 0, 4);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    break;
  }

  case PB::EventType::PartEvent: {
    PB::PartEvent* e = (PB::PartEvent*) _e;
    lua_createtable(L, 0, 5);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->reason.c_str());
    lua_setfield(L, -2, "reason");
    break;
  }

  case PB::EventType::NickEvent: {
    PB::NickEvent* e = (PB::NickEvent*) _e;
    lua_createtable(L, 0, 5);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->new_nick.c_str());
    lua_setfield(L, -2, "new_nick");
    break;
  }

  case PB::EventType::PRIVMSGEvent: {
    PB::PRIVMSGEvent* e = (PB::PRIVMSGEvent*) _e;
    lua_createtable(L, 0, 7);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->target.c_str());
    lua_setfield(L, -2, "target");
    lua_pushstring(L, e->reply_to.c_str());
    lua_setfield(L, -2, "reply_to");
    lua_pushstring(L, e->message.c_str());
    lua_setfield(L, -2, "message");
    break;
  }

  case PB::EventType::CTCPEvent: {
    PB::CTCPEvent* e = (PB::CTCPEvent*) _e;
    lua_createtable(L, 0, 7);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->target.c_str());
    lua_setfield(L, -2, "target");
    lua_pushstring(L, e->reply_to.c_str());
    lua_setfield(L, -2, "reply_to");
    lua_createtable(L, 0, e->split.size());
      int i = 1;
      for (std::string s : e->split) {
        lua_pushinteger(L, i);
        lua_pushstring(L, s.c_str());
        lua_settable(L, -3);
        ++i;
      }
    lua_setfield(L, -2, "split");
    break;
  }

  case PB::EventType::NOTICEEvent: {
    PB::NOTICEEvent* e = (PB::NOTICEEvent*) _e;
    lua_createtable(L, 0, 6);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->target.c_str());
    lua_setfield(L, -2, "target");
    lua_pushstring(L, e->message.c_str());
    lua_setfield(L, -2, "message");
    break;
  }

  case PB::EventType::CommandEvent: {
    PB::CommandEvent* e = (PB::CommandEvent*) _e;
    lua_createtable(L, 0, 9);
    lua_pushstring(L, e->socket->name.c_str());
    lua_setfield(L, -2, "socket");
    lua_pushstring(L, e->nick.c_str());
    lua_setfield(L, -2, "nick");
    lua_pushstring(L, e->user.c_str());
    lua_setfield(L, -2, "user");
    lua_pushstring(L, e->host.c_str());
    lua_setfield(L, -2, "host");
    lua_pushstring(L, e->target.c_str());
    lua_setfield(L, -2, "target");
    lua_pushstring(L, e->reply_to.c_str());
    lua_setfield(L, -2, "reply_to");
    lua_pushstring(L, e->message.c_str());
    lua_setfield(L, -2, "message");
    lua_pushstring(L, e->command.c_str());
    lua_setfield(L, -2, "command");
      lua_createtable(L, 0, e->split.size());
      int i = 1;
      for (std::string s : e->split) {
        lua_pushinteger(L, i);
        lua_pushstring(L, s.c_str());
        lua_settable(L, -3);
        ++i;
      }
      lua_setfield(L, -2, "split");
    break;
  }

  default:
    throw std::runtime_error("fired base event or there's no event handler for " + names[_e->type()]);
    break;
  }
}

void PB::LuaPlugin::handle_event(std::shared_ptr<Event> e) {
  std::lock_guard<std::mutex> lock(lmtx);

  lua_getglobal(L, "__pb_event_handlers");
  if (get_table_value(L, names[e->type()]) && lua_isfunction(L, -1)) {
    push_event_table(L, e.get());
    if (lua_pcall(L, 1, 0, 0))
      log.error("Lua error: %s", lua_tostring(L, -1));
  }

  lua_settop(L, 0);
}

int lfunc_command(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);

  std::string name;
  std::string group;
  PB::CommandInfo info;

  lua_pushnil(L);
  while (lua_next(L, 1)) {
    if (lua_isstring(L, 2)) {
      std::string key(lua_tostring(L, 2));
      if (lua_isnumber(L, 3)) {
        int value(lua_tonumber(L, 3));
        if (key == "level") {
          info.level = value;
        } else if (key == "cooldown") {
          info.cooldown = value;
        }
      } else if (lua_isstring(L, 3)) {
        std::string value(lua_tostring(L, 3));
        if (key == "name") {
          name = value;
        } else if (key == "group") {
          group = value;
        } else if (key == "description") {
          info.description = value;
        } else if (key == "usage") {
          info.usage = value;
        }
      } 
    }
    lua_pop(L, 1);
  }
  lua_pop(L, 1);

  info.handler = [self, name, group] (PB::CommandEvent* e) noexcept {
    std::lock_guard<std::mutex> lock(self->lmtx);

    lua_getglobal(self->L, "__pb_command_handlers");

    lua_getfield(self->L, -1, group.c_str());
    if (!lua_istable(self->L, -1)) {
      std::cerr << "__pb_command_handlers[group] is not table" << std::endl;
      lua_settop(self->L, 0);
      return;
    }
    lua_getfield(self->L, -1, name.c_str());
    if (!lua_isfunction(self->L, -1)) {
      std::cerr << "__pb_command_handlers[group][name] is not function" << std::endl;
      lua_settop(self->L, 0);
      return;
    }
    push_event_table(self->L, e);
    if (lua_pcall(self->L, 1, 0, 0))
      std::cerr << std::string(lua_tostring(self->L, -1)) << std::endl;

    lua_settop(self->L, 0);
  };

  (*self->commands)[group][name] = info;

  return 0;
}

static int lfunc_write(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2)) {
    lua_pushstring(L, "lfunc_write: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->write(std::string(lua_tostring(L, 2)));
  return 0;
}

static int lfunc_join(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2)) {
    lua_pushstring(L, "lfunc_join: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->join(std::string(lua_tostring(L, 2)));
  return 0;
}

static int lfunc_part(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_part: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->part(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_nick(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2)) {
    lua_pushstring(L, "lfunc_nick: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->change_nick(std::string(lua_tostring(L, 2)));
  return 0;
}

static int lfunc_who(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2)) {
    lua_pushstring(L, "lfunc_who: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->who(std::string(lua_tostring(L, 2)));
  return 0;
}

static int lfunc_privmsg(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_privmsg: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->privmsg(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_ctcp(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_ctcp: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->ctcp(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_notice(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_notice: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->notice(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_nctcp(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_nctcp: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->nctcp(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_mode(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_mode Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->mode(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_kick(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 4 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
    lua_pushstring(L, "lfunc_kick Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->kick(
    std::string(lua_tostring(L, 2)),
    std::string(lua_tostring(L, 3)),
    std::string(lua_tostring(L, 4))
  );
  return 0;
}

static int lfunc_remove(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 4 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
    lua_pushstring(L, "lfunc_remove Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->remove(
    std::string(lua_tostring(L, 2)),
    std::string(lua_tostring(L, 3)),
    std::string(lua_tostring(L, 4))
  );
  return 0;
}

static int lfunc_op(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_op: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->op(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_deop(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_deop: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->deop(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_hop(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_hop: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->hop(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_dehop(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_dehop: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->dehop(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_voice(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_voice: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->voice(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_devoice(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_devoice: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->devoice(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_quiet(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_quiet: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->quiet(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_unquiet(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_unquiet: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->unquiet(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_ban(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_ban: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->ban(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_unban(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_unban: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->unban(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_exempt(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_exempt: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->exempt(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_unexempt(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 3 || !lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
    lua_pushstring(L, "lfunc_unexempt: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->unexempt(std::string(lua_tostring(L, 2)), std::string(lua_tostring(L, 3)));
  return 0;
}

static int lfunc_get_host(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_host: Invalid arguments"); lua_error(L);
    return 0;
  }
  lua_pushstring(L, self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->host.c_str());
  return 1;
}

static int lfunc_get_port(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_port: Invalid arguments"); lua_error(L);
    return 0;
  }
  lua_pushnumber(L, self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->port);
  return 1;
}

static int lfunc_get_nick(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_nick: Invalid arguments"); lua_error(L);
    return 0;
  }
  lua_pushstring(L, self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->nick.c_str());
  return 1;
}

static int lfunc_get_user(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_user: Invalid arguments"); lua_error(L);
    return 0;
  }
  lua_pushstring(L, self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->user.c_str());
  return 1;
}

static int lfunc_get_pass(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_pass: Invalid arguments"); lua_error(L);
    return 0;
  }
  std::shared_ptr<PB::IRCSocket> sock = self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))];
  if (sock->has_pass) {
    lua_pushstring(L, sock->pass.c_str());
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int lfunc_get_autojoin(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_pass: Invalid arguments"); lua_error(L);
    return 0;
  }
  std::shared_ptr<PB::IRCSocket> sock = self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))];
  lua_createtable(L, 0, sock->autojoin.size());
  int i = 1;
  for (std::string s : sock->autojoin) {
    lua_pushinteger(L, i);
    lua_pushstring(L, s.c_str());
    lua_settable(L, -3);
    ++i;
  }
  return 1;
}

static int lfunc_get_autoowner(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_autoowner: Invalid arguments"); lua_error(L);
    return 0;
  }
  std::shared_ptr<PB::IRCSocket> sock = self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))];
  if (sock->autoowner == std::nullopt) {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, sock->autoowner.value_or("<UNKNOWN>").c_str());
  }
  return 1;
}

static int lfunc_get_prefix(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_get_prefix: Invalid arguments"); lua_error(L);
    return 0;
  }
  lua_pushstring(L, self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))]->prefix.c_str());
  return 1;
}

static int lfunc_get_user_info(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2)) {
    lua_pushstring(L, "lfunc_get_pass: Invalid arguments"); lua_error(L);
    return 0;
  }
  std::shared_ptr<PB::IRCSocket> sock = self->plugin_manager->bot->sockets[std::string(lua_tostring(L, 1))];
  PB::User u = sock->user_cache[std::string(lua_tostring(L, 2))];
  lua_createtable(L, 0, 3);
  lua_pushstring(L, lua_tostring(L, 2));
  lua_setfield(L, -2, "nick");
  if (u.user == std::nullopt) {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, u.user.value_or("<UNKNOWN>").c_str());
  }
  lua_setfield(L, -2, "user");
  if (u.host == std::nullopt) {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, u.host.value_or("<UNKNOWN>").c_str());
  }
  lua_setfield(L, -2, "host");
  return 1;
}

static int lfunc_log(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.write("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_debug(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_debug: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.debug("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_io(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_io: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.io("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_info(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_info: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.info("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_warning(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_warning: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.warning("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_error(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_error: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.error("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_important(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_important: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.important("%s", lua_tostring(L, 1));
  return 1;
}

static int lfunc_log_critical(lua_State* L) {
  PB::LuaPlugin* self = get_self(L);
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "lfunc_log_critical: Invalid arguments"); lua_error(L);
    return 0;
  }
  self->log.critical("%s", lua_tostring(L, 1));
  return 1;
}

PB::LuaPlugin::LuaPlugin(PB::PluginManager* _plugin_manager, std::string _name)
: PB::Plugin(_plugin_manager, _name, PB::Plugin::Type::Lua)
{
  commands = std::shared_ptr<PB::Plugin::CMap>(new PB::Plugin::CMap());
  load();
}

PB::LuaPlugin::~LuaPlugin() {
  unload();
}

void PB::LuaPlugin::load() {
  std::lock_guard<std::mutex> lock(lmtx);
  L = lua_open();
  luaL_openlibs(L);

  void** dp = (void**) lua_newuserdata(L, sizeof(this));
  *dp = this;
  lua_setglobal(L, "__pb_addr");

  lua_pushlightuserdata(L, plugin_manager->bot->db);
  lua_setglobal(L, "__pb_db");

  lua_newtable(L);
  lua_setglobal(L, "__pb_event_handlers");
  lua_newtable(L);
  lua_setglobal(L, "__pb_command_handlers");

  lua_register(L, "write", lfunc_write);
  lua_register(L, "join", lfunc_join);
  lua_register(L, "part", lfunc_part);
  lua_register(L, "nick", lfunc_nick);
  lua_register(L, "who", lfunc_who);
  lua_register(L, "privmsg", lfunc_privmsg);
  lua_register(L, "ctcp", lfunc_ctcp);
  lua_register(L, "notice", lfunc_notice);
  lua_register(L, "nctcp", lfunc_nctcp);
  lua_register(L, "mode", lfunc_mode);
  lua_register(L, "kick", lfunc_kick);
  lua_register(L, "remove", lfunc_remove);
  lua_register(L, "op", lfunc_op);
  lua_register(L, "deop", lfunc_deop);
  lua_register(L, "hop", lfunc_hop);
  lua_register(L, "dehop", lfunc_dehop);
  lua_register(L, "voice", lfunc_voice);
  lua_register(L, "devoice", lfunc_devoice);
  lua_register(L, "quiet", lfunc_quiet);
  lua_register(L, "unquiet", lfunc_unquiet);
  lua_register(L, "ban", lfunc_ban);
  lua_register(L, "unban", lfunc_unban);
  lua_register(L, "exempt", lfunc_exempt);
  lua_register(L, "unexempt", lfunc_unexempt);
  lua_register(L, "get_host", lfunc_get_host);
  lua_register(L, "get_port", lfunc_get_port);
  lua_register(L, "get_nick", lfunc_get_nick);
  lua_register(L, "get_user", lfunc_get_user);
  lua_register(L, "get_pass", lfunc_get_pass);
  lua_register(L, "get_autojoin", lfunc_get_autojoin);
  lua_register(L, "get_autoowner", lfunc_get_autoowner);
  lua_register(L, "get_prefix", lfunc_get_prefix);
  lua_register(L, "get_user_info", lfunc_get_user_info);
  lua_register(L, "log", lfunc_log);
  lua_register(L, "log_debug", lfunc_log_debug);
  lua_register(L, "log_io", lfunc_log_io);
  lua_register(L, "log_info", lfunc_log_info);
  lua_register(L, "log_warning", lfunc_log_warning);
  lua_register(L, "log_error", lfunc_log_error);
  lua_register(L, "log_important", lfunc_log_important);
  lua_register(L, "log_critical", lfunc_log_critical);

  if(luaL_dostring(L,
    "sqlite3 = require('lsqlite3')\n" \
    "db = sqlite3.open_ptr(__pb_db)\n" \
    "function on(name, handler)\n" \
      "__pb_event_handlers[name] = handler\n" \
    "end\n" \
    "function command(params, handler)\n" \
      "if params['name'] == nil then\n" \
        "error('no command name')\n" \
      "end\n" \
      "if params['group'] == nil then\n" \
        "error('no command group')\n" \
      "end\n" \
      "if __pb_command_handlers[params['group']] == nil then\n" \
        "__pb_command_handlers[params['group']] = {}" \
      "end\n" \
      "__pb_command_handlers[params['group']][params['name']] = handler\n" \
      "__pb_register_command(params)\n" \
    "end\n" \
    "function reply(e, message)\n" \
      "privmsg(e.socket, e.reply_to, message)\n"\
    "end\n" \
    "function nreply(e, message)\n" \
      "notice(e.socket, e.nick, message)\n"
    "end\n"
  )) throw std::runtime_error(std::string(lua_tostring(L, -1)));
  
  lua_register(L, "__pb_register_command", lfunc_command);

  if (luaL_dofile(L, find_path_to_plugin(name).c_str()))
    throw std::runtime_error(std::string(lua_tostring(L, -1)));

  lua_settop(L, 0);
}

void PB::LuaPlugin::unload() {
  std::lock_guard<std::mutex> lock(lmtx);
  commands = std::shared_ptr<PB::Plugin::CMap>(new PB::Plugin::CMap());
  lua_close(L);
}