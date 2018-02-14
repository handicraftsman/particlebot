#include <particlebot.hpp>

pb_plugin();

bool get_help_message(PB::Bot* bot, std::string cmdname, std::string& msg) {
  for (auto& p : bot->plugin_manager.plugins) {
    for (auto& g : (*p.second->commands)) {
      for (auto& c : g.second) {
        if (c.first == cmdname) {
          // plugin -> group -> command usage | description | level | cooldown
          msg = 
            p.first + " -> " + g.first + " -> " + c.first + " " + c.second.usage
          + " | " + c.second.description
          + " | level=" + std::to_string(c.second.level)
          + " | cooldown=" + std::to_string(c.second.cooldown);
          return true;
        }
      }
    }
  }
  return false;
}

void cmdfunc_help(PB::CommandEvent* event) noexcept {
  if (event->split.empty()) {
    event->nreply(
      "Do `" + event->socket->prefix + "list` to list all command groups. "
    + "Do `" + event->socket->prefix + "list <group>` to list all commands in the given group. "
    + "Do `" + event->socket->prefix + "help <command>` to display help message for the given command. ");
    return;
  }

  if (event->split.size() > 5) return;

  for (std::string cmdname : event->split) {
    std::string msg;
    if (get_help_message(event->bot, cmdname, msg)) {
      event->nreply(msg);
    }
  }
}

void cmdfunc_list(PB::CommandEvent* event) noexcept {
  if (event->split.empty()) {
    std::string lst("Groups: ");
    for (auto& p : event->bot->plugin_manager.plugins) {
      for (auto& g : (*p.second->commands)) {
        lst += g.first + " ";
      }
    }
    event->nreply(lst);
    return;
  }

  if (event->split.size() > 5) return;

  for (std::string groupname : event->split) {
    for (auto& p : event->bot->plugin_manager.plugins) {
      for (auto& g : (*p.second->commands)) {
        if (g.first == groupname) {
          std::string lst(p.first + " -> " + groupname + ": ");
          for (auto& c : g.second) {
            lst += c.first + " ";
          }
          event->nreply(lst);
        }
      }
    }
  }
}