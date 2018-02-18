#include <particlebot.hpp>

pb_plugin();
pb_plugin_main();

/* Debug */
void cmdfunc_cache(PB::CommandEvent* event) noexcept;
void cmdfunc_echo(PB::CommandEvent* event) noexcept;
void cmdfunc_reload(PB::CommandEvent* event) noexcept;

/* Help */
void cmdfunc_help(PB::CommandEvent* event) noexcept;
void cmdfunc_list(PB::CommandEvent* event) noexcept;

/* Admin */
void cmdfunc_perms(PB::CommandEvent* event) noexcept;
void cmdfunc_raw(PB::CommandEvent* event) noexcept;
void cmdfunc_nick(PB::CommandEvent* event) noexcept;
void cmdfunc_join(PB::CommandEvent* event) noexcept;
void cmdfunc_part(PB::CommandEvent* event) noexcept;
void cmdfunc_flushq(PB::CommandEvent* event) noexcept;


extern "C" __attribute__((used)) bool pb_init(PB::Bot* bot) noexcept {
  /* Debug */
  PB::CommandInfo& cmd_cache = register_command("debug", "cache", cmdfunc_cache);
  cmd_cache.description = "Checks user cache";
  cmd_cache.usage = "[target...]";
  cmd_cache.level = 4;

  PB::CommandInfo& cmd_echo = register_command("debug", "echo", cmdfunc_echo);
  cmd_echo.description = "Replies with the given message";
  cmd_echo.usage = "[message...]";
  cmd_echo.cooldown = 10;

  PB::CommandInfo& cmd_reload = register_command("debug", "reload", cmdfunc_reload);
  cmd_reload.description = "Reloads all plugins";
  cmd_reload.usage = "";
  cmd_reload.level = 4;

  /* Help */
  PB::CommandInfo& cmd_help = register_command("help", "help", cmdfunc_help);
  cmd_help.description = "Shows help info";
  cmd_help.usage = "[command...]";
  cmd_help.cooldown = 2;

  PB::CommandInfo& cmd_list = register_command("help", "list", cmdfunc_list);
  cmd_list.description = "Shows group&command lists";
  cmd_list.usage = "[group...]";
  cmd_list.cooldown = 2;

  /* Admin */
  PB::CommandInfo& cmd_perms = register_command("admin", "perms", cmdfunc_perms);
  cmd_perms.description = "Manages user permissions";
  cmd_perms.usage = "[who [level]]";
  cmd_perms.level = 4;

  PB::CommandInfo& cmd_raw = register_command("admin", "raw", cmdfunc_raw);
  cmd_raw.description = "Sends given message to the server";
  cmd_raw.usage = "[message...]";
  cmd_raw.level = 4;

  PB::CommandInfo& cmd_nick = register_command("admin", "nick", cmdfunc_nick);
  cmd_nick.description = "Changes bot's nickname";
  cmd_nick.usage = "<new_nick>";
  cmd_nick.level = 3;

  PB::CommandInfo& cmd_join = register_command("admin", "join", cmdfunc_join);
  cmd_join.description = "Joins channels";
  cmd_join.usage = "<chan...>";
  cmd_join.level = 3;

  PB::CommandInfo& cmd_part = register_command("admin", "part", cmdfunc_part);
  cmd_part.description = "Parts a channel";
  cmd_part.usage = "<chan> [reason...]";
  cmd_part.level = 3;

  PB::CommandInfo& cmd_flushq = register_command("admin", "flushq", cmdfunc_flushq);
  cmd_flushq.description = "Flushes event queue";
  cmd_flushq.usage = "";
  cmd_flushq.cooldown = 5;

  return true;
}