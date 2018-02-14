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