#include <particlebot.hpp>

pb_plugin();

void cmdfunc_cache(PB::CommandEvent* event) noexcept {
  if (event->split.empty()) {
    event->nreply(std::to_string(event->socket->user_cache.size()) + " users in the cache");
  } else if (event->split.size() <= 5) {
    for (std::string n : event->split) {
      PB::User& u = event->socket->user_cache[n];
      event->nreply(
        "User query=[" + n
          + "] nick=[" + u.nick.value_or("<UNKNOWN>")
          + "] user=[" + u.user.value_or("<UNKNOWN>")
          + "] host=[" + u.host.value_or("<UNKNOWN>")
          + "]"
      );
    }
  }
}

void cmdfunc_echo(PB::CommandEvent* event) noexcept {
  std::string msg = "";
  for (auto s : event->split) {
    msg += s + " ";
  }
  msg.pop_back();
  event->socket->privmsg(event->reply_to, msg);
}

void cmdfunc_reload(PB::CommandEvent* event) noexcept {
  for (auto _p : event->bot->plugin_manager.plugins) {
    if (_p.second->type == PB::Plugin::Type::Lua) {
      PB::LuaPlugin* p = (PB::LuaPlugin*) _p.second.get();
      p->unload();
      p->load();
    }
  }
  event->nreply("Done reloading all lua plugins!");
}