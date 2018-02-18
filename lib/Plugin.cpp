#include "include.hpp"

PB::Plugin::Plugin(PB::PluginManager* _plugin_manager, std::string _name, Type _type)
: name(_name)
, type(_type)
, plugin_manager(_plugin_manager)
, log((type == PB::Plugin::Type::CPP ? "?" : "@") + name)
{
  log.write("Hello, %s!", name.c_str());
}

PB::Plugin::~Plugin() {
  
}

bool PB::Plugin::handle_command(PB::CommandEvent* e) {
  for (auto& g : (*commands)) {
    for (auto& c : g.second) {
      if (c.first == e->command) {
        int lvl = plugin_manager->bot->get_level(e->socket->name, e->host);
        if (lvl >= c.second.level) {
          auto last = c.second.last_uses[e->socket->name][e->nick][c.first];
          auto current = std::chrono::system_clock::now();
          if (lvl >= 3 || (current - last) > std::chrono::seconds(c.second.cooldown)) {
            c.second.handler(e);
            c.second.last_uses[e->socket->name][e->nick][c.first] = current;
          }
        } else {
          e->nreply("Error: your permission level is " + std::to_string(lvl) + ", while at least " + std::to_string(c.second.level) +" is required!");
        }
        return true;
      }
    }
  }
  return false;
}