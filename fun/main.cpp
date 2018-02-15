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
  static std::vector<std::string> qualities {
    "",
    "%C?LBLUEuncommon ",
    "%C?BLUErare ",
    "%C?PURPLEepic ",
    "%C?YELLOWlegendary ",
    "%C?ORANGEholy ",
    "%C?CYANhi-tech ",
    "%C?LBLUEquantum ",
    "%C?BLACKevil ",
    "%C?PURPLEmagical ",
    "%C?LBLUEancient ",
    "%C?REDvampiric ",
  };

  static std::vector<std::string> types {
    "",
    "%C?ORANGEblazing ",
    "%C?REDhot ",
    "%C?CYANfrost ",
    "%C?LBLUEchilling ",
    "%C?YELLOWshocking ",
    "%C?LGREYaerial ",
    "%C?GREYstone ",
    "%C?BROWNmud ",
    "%C?BLACKvoid ",
    "%C?WHITEghostly ",
    "%C?REDbloody ",
    "%C?REDn%C?GREENy%C?BLUEa%C?CYANn ",
    "%C?CYANteleporting ",
    "%C?BROWNwild ",
    "%C?GREENalien ",
    "%C?BLACKspacious ",
    "%C?REDatomic ",
    "%C?BROWNchocolate "
  };

  if (event->split.size() > 1) {
    event->nreply("Error: invalid argument amount");
    return;
  }

  std::string quality = *select_randomly(qualities.begin(), qualities.end());
  std::string type = *select_randomly(types.begin(), types.end());

  if (event->split.empty()) {
    event->socket->ctcp(event->reply_to, "ACTION &Ngives &B" + event->nick + "&N a &B" + quality + type + "&N&B%C?BROWNcookie&N");
  } else {
    event->socket->ctcp(event->reply_to, "ACTION &Ngives &B" + event->split[0] + "&N a &B" + quality + type + "&N&B%C?BROWNcookie&N");
  }
}

extern "C" __attribute__((used)) bool pb_init(PB::Bot* bot) noexcept {
  PB::CommandInfo& cmd_poke = register_command("fun", "poke", cmdfunc_poke);
  cmd_poke.description = "Pokes target";
  cmd_poke.usage = "[target]";
  cmd_poke.cooldown = 5;

  PB::CommandInfo& cmd_cookie = register_command("fun", "cookie", cmdfunc_cookie);
  cmd_cookie.description = "Gives a cookie to the target";
  cmd_cookie.usage = "[target]";
  cmd_cookie.cooldown = 5;

  return true;
}