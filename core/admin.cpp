#include <particlebot.hpp>

pb_plugin();

void cmdfunc_perms(PB::CommandEvent* event) noexcept {
  if (event->split.empty()) {
    int lvl = event->bot->get_level(event->socket->name, event->host);
    event->nreply("Your permission level is " + std::to_string(lvl));
  } else if (event->split.size() == 1) {
    std::string who = event->socket->user_cache[event->split[0]].host.value_or(event->split[0]);
    int lvl = event->bot->get_level(event->socket->name, who);
    event->nreply(who + "'s permission level is " + std::to_string(lvl));
  } else if (event->split.size() == 2) {
    std::string who = event->socket->user_cache[event->split[0]].host.value_or(event->split[0]);
    int lvl = std::stoi(event->split[1]);
    if (lvl > 4) {
      event->nreply("Error: max permission level is 4");
      return;
    }
    event->bot->set_level(event->socket->name, who, lvl);
    event->nreply(who + "'s permission level is now " + event->split[1]);
  } else {
    event->nreply("Error: invalid argument amount");
  }
}

void cmdfunc_raw(PB::CommandEvent* event) noexcept {
  std::string msg = "";
  for (auto s : event->split) {
    msg += s + " ";
  }
  msg.pop_back();
  event->socket->write(msg);
}

void cmdfunc_join(PB::CommandEvent* event) noexcept {
  if (event->split.empty() || event->split.size() > 5) {
    event->nreply("Error: invalid argument amount");
    return;
  }

  for (std::string channame : event->split) {
    event->socket->join(channame);
  }
}

void cmdfunc_part(PB::CommandEvent* event) noexcept {
  if (event->split.empty()) {
    event->nreply("Error: invalid argument amount");
    return;
  }

  std::string chan = event->split[0];
  std::string reason;
  if (event->split.size() == 1) {
    reason = "Bye";
  } else {
    for (auto it = event->split.begin() + 1; it != event->split.end(); ++it) {
      reason += *it + " ";
    }
    reason.pop_back();
  }

  event->socket->part(chan, reason);
}