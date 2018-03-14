#include <particlebot.hpp>

pb_plugin();
pb_plugin_main();

void cmdfunc_poke(PB::CommandEvent* event) noexcept {
  std::string target;
  if (event->split.empty()) {
    target = event->nick;
  } else if (event->split.size() == 1) {
    target = event->split[0];
  } else {
    event->nreply("Error: invalid argument amount");
    return;
  }
  event->socket->ctcp(event->reply_to, "ACTION pokes " + target);
}

template<typename Iter, typename RandomGenerator>
static Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
  std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
  std::advance(start, dis(g));
  return start;
}

template<typename Iter>
static Iter select_randomly(Iter start, Iter end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return select_randomly(start, end, gen);
}

void cmdfunc_cookie(PB::CommandEvent* event) noexcept {
  static std::map<std::string, std::string> qualities {
    {"normal", ""},
    {"uncommon", "%C?LBLUEuncommon "},
    {"rare", "%C?BLUErare "},
    {"epic", "%C?PURPLEepic "},
    {"legendary", "%C?YELLOWlegendary "},
    {"holy", "%C?ORANGEholy "},
    {"hi-tech", "%C?CYANhi-tech "},
    {"quantum", "%C?LBLUEquantum "},
    {"evil", "%C?BLACKevil "},
    {"magical", "%C?PURPLEmagical "},
    {"ancient", "%C?LBLUEancient "},
    {"vampiric", "%C?REDvampiric "}
  };

  static std::map<std::string, std::string> types {
    {"normal", ""},
    {"blazing", "%C?ORANGEblazing "},
    {"hot", "%C?REDhot "},
    {"frozen", "%C?CYANfrozen "},
    {"chilling", "%C?LBLUEchilling "},
    {"shocking", "%C?YELLOWshocking "},
    {"aerial", "%C?LGREYaerial "},
    {"stone", "%C?GREYstone "},
    {"mud", "%C?BROWNmud "},
    {"void", "%C?BLACKvoid "},
    {"ghostly", "%C?WHITEghostly "},
    {"bloody", "%C?REDbloody "},
    {"nyan", "%C?REDn%C?GREENy%C?BLUEa%C?CYANn "},
    {"teleporting", "%C?CYANteleporting "},
    {"wild", "%C?BROWNwild "},
    {"alient", "%C?GREENalien "},
    {"spacious", "%C?BLACKspacious "},
    {"atomic", "%C?REDatomic "},
    {"chocolate", "%C?BROWNchocolate" }
  };

  std::string target;
  std::string quality;
  std::string type;

  bool has_target = false;
  bool has_quality = false;
  bool has_type = false;

  for (std::string s : event->split) {
    if (s.substr(0,9) == "--quality") {
      quality = qualities[s.substr(10, s.size())];
      has_quality = true;
      continue;
    }
    if (s.substr(0,6) == "--type") {
      type = types[s.substr(7, s.size())];
      has_type = true;
      continue;
    }
    if (!target.empty())
      target += " ";
    target += s;
    has_target = true;
  }

  if (!has_quality)
    quality = select_randomly(qualities.begin(), qualities.end())->second;
  if (!has_type)
    type = select_randomly(types.begin(), types.end())->second;
  if (!has_target) {
    if (event->reply_to == event->socket->nick) {
      target = event->target;
    } else {
      target = event->nick;
    }
  }

  if (event->split.empty()) {
    event->socket->ctcp(event->reply_to, "ACTION &Ngives &B" + target + "&N a &B" + quality + type + "&N&B%C?BROWNcookie&N");
  } else {
    event->socket->ctcp(event->reply_to, "ACTION &Ngives &B" + target + "&N a &B" + quality + type + "&N&B%C?BROWNcookie&N");
  }
}

extern "C" __attribute__((used)) bool pb_init(PB::Bot* bot) noexcept {
  PB::CommandInfo& cmd_poke = register_command("fun", "poke", cmdfunc_poke);
  cmd_poke.description = "Pokes target";
  cmd_poke.usage = "[target]";
  cmd_poke.cooldown = 5;

  PB::CommandInfo& cmd_cookie = register_command("fun", "cookie", cmdfunc_cookie);
  cmd_cookie.description = "Gives a cookie to the target";
  cmd_cookie.usage = "[target] [--quality=quality] [--type=type]";
  cmd_cookie.cooldown = 5;

  return true;
}