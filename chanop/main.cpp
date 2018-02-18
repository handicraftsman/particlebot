#include <particlebot.hpp>

pb_plugin();
pb_plugin_main();

void cmdfunc_mode(PB::CommandEvent* event) noexcept {
  if (event->split.size() < 1) {
    event->nreply("Error: invalid argument amount");
    return;
  }

  std::string channel;
  int offset = 0;
  if (event->split[0][0] == '#' || event->split[0][0] == '!' || event->split[0][0] == '&') {
    channel = event->split[0];
    offset = 1;
  } else {
    channel = event->target;
  }
  
  std::string args = "";
  for (auto it = event->split.begin()+offset; it != event->split.end(); ++it) {
    args += *it + " ";
  }
  args.pop_back();

  event->socket->mode(channel, args);
};

struct ActArgs {
  std::string channel;
  std::string target;
  std::string t;
  std::string reason;
};

bool get_act_args(PB::CommandEvent* event, ActArgs& args, bool get_host = false, bool can_be_empty = false) {
  if (event->split.empty()) {
    if (can_be_empty) {
      args.channel = event->target;
      args.t = event->nick;
      if (get_host) {
        args.target = event->socket->user_cache[event->nick].host.value_or(event->nick);
      } else {
        args.target = event->nick;
      }
      args.reason = "Bye";
      return true;
    } else {
      return false;
    }
  }

  int offset = 0;
  if (event->split[0][0] == '#' || event->split[0][0] == '!' || event->split[0][0] == '&') {
    if (event->split.size() == 1) return false;
    args.channel = event->split[0];
    offset = 1;
  } else {
    args.channel = event->target;
  }

  args.t = event->split[offset];
  if (get_host) {
    args.target = event->socket->user_cache[event->split[offset]].host.value_or(event->split[offset]);
  } else {
    args.target = event->split[offset];
  }

  if (event->split.size() > (size_t) (1 + offset)) {
    for (auto it = event->split.begin()+1+offset; it != event->split.end(); ++it) {
      args.reason += *it + " ";
    }
    args.reason.pop_back();
  } else {
    args.reason = "Bye";
  }

  return true;
}

void cmdfunc_op(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->op(args.channel, args.target);
}

void cmdfunc_deop(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->deop(args.channel, args.target);
}

void cmdfunc_hop(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->hop(args.channel, args.target);
}

void cmdfunc_dehop(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->dehop(args.channel, args.target);
}

void cmdfunc_voice(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->voice(args.channel, args.target);
}

void cmdfunc_devoice(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, false, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->devoice(args.channel, args.target);
}

void cmdfunc_quiet(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true, false)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->quiet(args.channel, args.target);
}

void cmdfunc_unquiet(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true, false)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->unquiet(args.channel, args.target);
}

void cmdfunc_ban(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->ban(args.channel, args.target);
}

void cmdfunc_unban(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->unban(args.channel, args.target);
}

void cmdfunc_kban(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->ban(args.channel, args.target);
  event->socket->kick(args.channel, args.t, args.reason);
}

void cmdfunc_exempt(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->exempt(args.channel, args.target);
}

void cmdfunc_unexempt(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args, true)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->unexempt(args.channel, args.target);
}

void cmdfunc_kick(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->kick(args.channel, args.target, args.reason);
}

void cmdfunc_remove(PB::CommandEvent* event) noexcept {
  ActArgs args;
  if (!get_act_args(event, args)) {
    event->nreply("Error: invalid arguments");
    return;
  }
  event->socket->remove(args.channel, args.target, args.reason);
}


extern "C" __attribute__((used)) bool pb_init(PB::Bot* bot) noexcept {
  PB::CommandInfo& cmd_mode = register_command("chanop", "mode", cmdfunc_mode);
  cmd_mode.description = "Sets channel/user modes";
  cmd_mode.usage = "[channel] (<mode> [params...])...";
  cmd_mode.level = 2;

  PB::CommandInfo& cmd_op = register_command("chanop", "op", cmdfunc_op);
  cmd_op.description = "Ops target in the given channel";
  cmd_op.usage = "[channel] [target]";
  cmd_op.level = 2;

  PB::CommandInfo& cmd_deop = register_command("chanop", "deop", cmdfunc_deop);
  cmd_deop.description = "Deops target in the given channel";
  cmd_deop.usage = "[channel] [target]";
  cmd_deop.level = 2;

  PB::CommandInfo& cmd_hop = register_command("chanop", "hop", cmdfunc_hop);
  cmd_hop.description = "Half-ops target in the given channel";
  cmd_hop.usage = "[channel] [target]";
  cmd_hop.level = 2;

  PB::CommandInfo& cmd_dehop = register_command("chanop", "dehop", cmdfunc_dehop);
  cmd_dehop.description = "De-half-ops target in the given channel";
  cmd_dehop.usage = "[channel] [target]";
  cmd_dehop.level = 2;

  PB::CommandInfo& cmd_quiet = register_command("chanop", "quiet", cmdfunc_quiet);
  cmd_quiet.description = "Quiets target in the given channel";
  cmd_quiet.usage = "[channel] [target]";
  cmd_quiet.level = 2;

  PB::CommandInfo& cmd_unquiet = register_command("chanop", "unquiet", cmdfunc_unquiet);
  cmd_unquiet.description = "Unquiets target in the given channel";
  cmd_unquiet.usage = "[channel] [target]";
  cmd_unquiet.level = 2;

  PB::CommandInfo& cmd_ban = register_command("chanop", "ban", cmdfunc_ban);
  cmd_ban.description = "Bans target in given channel";
  cmd_ban.usage = "[channel] <target>";
  cmd_ban.level = 2;

  PB::CommandInfo& cmd_unban = register_command("chanop", "unban", cmdfunc_unban);
  cmd_unban.description = "Unbans target in the given channel";
  cmd_unban.usage = "[channel] <target>";
  cmd_unban.level = 2;

  PB::CommandInfo& cmd_kban = register_command("chanop", "kban", cmdfunc_kban);
  cmd_kban.description = "Kick-bans target in the given channel";
  cmd_kban.usage = "[channel] <target>";
  cmd_kban.level = 2;

  PB::CommandInfo& cmd_exempt = register_command("chanop", "exempt", cmdfunc_exempt);
  cmd_exempt.description = "Exempts target in given channel";
  cmd_exempt.usage = "[channel] <target>";
  cmd_exempt.level = 2;

  PB::CommandInfo& cmd_unexempt = register_command("chanop", "unexempt", cmdfunc_unexempt);
  cmd_unexempt.description = "Unexempts target in the given channel";
  cmd_unexempt.usage = "[channel] <target>";
  cmd_unexempt.level = 2;

  PB::CommandInfo& cmd_kick = register_command("chanop", "kick", cmdfunc_kick);
  cmd_kick.description = "Kicks target from the given channel";
  cmd_kick.usage = "[channel] <target> [reason]";
  cmd_kick.level = 2;

  PB::CommandInfo& cmd_remove = register_command("chanop", "remove", cmdfunc_remove);
  cmd_remove.description = "Removes target from the given channel";
  cmd_remove.usage = "[channel] <target> [reason]";
  cmd_remove.level = 2;

  return true;
}