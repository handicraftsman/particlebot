#include "include.hpp"

using namespace std::regex_constants;

/*
 * Base class (Event)
 */

PB::Event::Event()
: log("Event")
{}

PB::Event::~Event() {}

/*
 * Test (TestEvent)
 */

PB::TestEvent::TestEvent(std::string _str)
: str(_str)
{}

void PB::TestEvent::handler() {
  log.debug("%s", to_s().c_str());
}

std::string PB::TestEvent::to_s() {
  return "TestEvent str=[" + str + "]";
}

PB::EventType PB::TestEvent::type() {
  return PB::EventType::TestEvent;
}

/*
 * Connect (ConnectEvent)
 */

PB::ConnectEvent::ConnectEvent(IRCSocket* _socket)
: socket(_socket)
{}

void PB::ConnectEvent::handler() {
  for (std::string chan : socket->autojoin) {
    socket->join(chan);
  }
}

std::string PB::ConnectEvent::to_s() {
  return "ConnectEvent server=[" + socket->name + "]";
}

PB::EventType PB::ConnectEvent::type() {
  return PB::EventType::ConnectEvent;
}

/*
 * Disconnect (DisconnectEvent)
 */

PB::DisconnectEvent::DisconnectEvent(IRCSocket* _socket)
: socket(_socket)
{}

void PB::DisconnectEvent::handler() {
  std::thread([this] () {
    socket->log.important("Disconnected");
    if (socket->reconnects < 5) {
      socket->log.important("Reconnecting in 5 seconds");
      std::this_thread::sleep_for(std::chrono::seconds(5));
      ++(socket->reconnects);
      socket->connect();
    } else {
      socket->log.important("Reconnection limit reached");
    }
  }).detach();
}

std::string PB::DisconnectEvent::to_s() {
  return "DisconnectEvent server=[" + socket->name + "]";
}

PB::EventType PB::DisconnectEvent::type() {
  return PB::EventType::DisconnectEvent;
}

/*
 * IRC message (IRCMessageEvent)
 */

PB::IRCMessageEvent::IRCMessageEvent(IRCSocket* _socket, std::string _raw_msg)
: socket(_socket)
, raw_msg(_raw_msg)
{}

static std::regex regex_code("^:.+? (\\d\\d\\d) .+? (.+)", ECMAScript | icase);
static std::regex regex_ping("^PING :(.+)", ECMAScript | icase);
static std::regex regex_join("^:(.+?)!(.+?)@(.+?) JOIN (.+)", ECMAScript | icase);
static std::regex regex_part("^:(.+?)!(.+?)@(.+?) PART (.+?) :(.+)", ECMAScript | icase);
static std::regex regex_privmsg("^:(.+?)!(.+?)@(.+?) PRIVMSG (.+?) :(.+)", ECMAScript | icase);
static std::regex regex_notice("^:(.+?)!(.+?)@(.+?) NOTICE (.+?) :(.+)", ECMAScript | icase);
static std::regex regex_nick("^:(.+?)!(.+?)@(.+?) NICK :(.+)", ECMAScript | icase);

void PB::IRCMessageEvent::handler() {
  socket->log.io("R> %s", raw_msg.c_str());
  
  auto m = std::smatch {};

  if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_privmsg))) {
    event_machine->fire(new PB::PRIVMSGEvent(socket, m[1], m[2], m[3], m[4], m[5]));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_notice))) {
    event_machine->fire(new PB::NOTICEEvent(socket, m[1], m[2], m[3], m[4], m[5]));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_code))) {
    event_machine->fire(new PB::CodeEvent(socket, m[1].str(), m[2].str()));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_ping))) {
    event_machine->fire(new PB::PingEvent(socket, m[1].str()));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_join))) {
    event_machine->fire(new PB::JoinEvent(socket, m[4], m[1], m[2], m[3]));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_part))) {
    event_machine->fire(new PB::PartEvent(socket, m[4], m[1], m[2], m[3], m[5]));
  } else if ((m = std::smatch{}, std::regex_match(raw_msg, m, regex_nick))) {
    event_machine->fire(new PB::NickEvent(socket, m[1], m[2], m[3], m[4]));
  }
}

std::string PB::IRCMessageEvent::to_s() {
  return "IRCMessageEvent server=[" + socket->name + "] msg=[" + raw_msg + "]";
}

PB::EventType PB::IRCMessageEvent::type() {
  return PB::EventType::IRCMessageEvent;
}

/*
 * Code (CodeEvent)
 */

PB::CodeEvent::CodeEvent(IRCSocket* _socket, std::string _code, std::string _extra)
: socket(_socket)
, code(_code)
, extra(_extra)
{}

void PB::CodeEvent::handler() {
  if (code == "001") {
    event_machine->fire(new ConnectEvent(socket));
  } else if (code == "352") {
    // <channel> <user> <host> <server> <nick> ( "H" / "G" > ["*"] [ ( "@" / "+" ) ] :<hopcount> <real name>
    static std::regex regex_whoreply("^.+? (.+?) (.+?) .+? (.+?) .*");
    auto m = std::smatch {};
    if (std::regex_match(extra, m, regex_whoreply)) {
      event_machine->fire(new PB::WHOREPLYEvent(socket, m[3], m[1], m[2]));
    }
  }
}

std::string PB::CodeEvent::to_s() {
  return "CodeEvent server=[" + socket->name + "] code=[" + code + "] extra=[" + extra + "]";
}

PB::EventType PB::CodeEvent::type() {
  return PB::EventType::CodeEvent;
}

/*
 * Ping (PingEvent)
 */

PB::PingEvent::PingEvent(IRCSocket* _socket, std::string _target)
: socket(_socket)
, target(_target)
{}

void PB::PingEvent::handler() {
  socket->write("PONG :" + target);
}

std::string PB::PingEvent::to_s() {
  return "PingEvent server=[" + socket->name + "] target=[" + target + "]";
}

PB::EventType PB::PingEvent::type() {
  return PB::EventType::PingEvent;
}

/*
 * WHOREPLY (WHOREPLYEvent)
 */

PB::WHOREPLYEvent::WHOREPLYEvent(IRCSocket* _socket, std::string _nick, std::string _user, std::string _host)
: socket(_socket)
, nick(_nick)
, user(_user)
, host(_host)
{}

void PB::WHOREPLYEvent::handler() {
  socket->user_cache[nick].nick = nick;
  socket->user_cache[nick].user = user;
  socket->user_cache[nick].host = host;
}

std::string PB::WHOREPLYEvent::to_s() {
  return "WHOREPLYEvent server=[" + socket->name + "] nick=[" + nick + "] user=[" + user + "] host=[" + host + "]";
}

PB::EventType PB::WHOREPLYEvent::type() {
  return PB::EventType::WHOREPLYEvent;
}

/*
 * Join (JoinEvent)
 */

PB::JoinEvent::JoinEvent(IRCSocket* _socket, std::string _channel, std::string _nick, std::string _user, std::string _host)
: socket(_socket)
, channel(_channel)
, nick(_nick)
, user(_user)
, host(_host)
{}

void PB::JoinEvent::handler() {
  if (nick == socket->nick) {
    socket->who(channel);
  }
  socket->user_cache[nick].nick = nick;
  socket->user_cache[nick].user = user;
  socket->user_cache[nick].host = host;
}

std::string PB::JoinEvent::to_s() {
  return "JoinEvent server=[" + socket->name + "] channel=[" + channel + "] nick=[" + nick + "] user=" + user + "] host=[" + host + "]";
}

PB::EventType PB::JoinEvent::type() {
  return PB::EventType::JoinEvent;
}

/*
 * Part (PartEvent)
 */

PB::PartEvent::PartEvent(IRCSocket* _socket, std::string _channel, std::string _nick, std::string _user, std::string _host, std::string _reason)
: socket(_socket)
, channel(_channel)
, nick(_nick)
, user(_user)
, host(_host)
, reason(_reason)
{}

void PB::PartEvent::handler() {}

std::string PB::PartEvent::to_s() {
  return 
      "PartEvent server=[" + socket->name
    + "] channel=[" + channel 
    + "] nick=["+ nick
    + "] user=[" + user
    + "] host=[" + host
    + "] reason=[" + reason
    + "]";
}

PB::EventType PB::PartEvent::type() {
  return PB::EventType::PartEvent;
}

/*
 * Nick (NickEvent)
 */

PB::NickEvent::NickEvent(IRCSocket* _socket, std::string _nick, std::string _user, std::string _host, std::string _new_nick)
: socket(_socket)
, nick(_nick)
, user(_user)
, host(_host)
, new_nick(_new_nick)
{}

void PB::NickEvent::handler() {
  if (new_nick == socket->nick) socket->nick = new_nick;
  socket->user_cache[new_nick].nick = new_nick;
  socket->user_cache[new_nick].user = user;
  socket->user_cache[new_nick].host = host;
  socket->user_cache[nick] = PB::User();
}

std::string PB::NickEvent::to_s() {
  return 
      "NickEvent server=[" + socket->name
    + "] nick=["+ nick
    + "] user=[" + user
    + "] host=[" + host
    + "] new_nick=[" + new_nick
    + "]";
}

PB::EventType PB::NickEvent::type() {
  return PB::EventType::NickEvent;
}

/*
 * PRIVMSG (PRIVMSGEvent)
 */

PB::PRIVMSGEvent::PRIVMSGEvent(IRCSocket* _socket, std::string _nick, std::string _user, std::string _host, std::string _target, std::string _message)
: socket(_socket)
, nick(_nick)
, user(_user)
, host(_host)
, target(_target)
, message(_message)
{}

void PB::PRIVMSGEvent::handler() {
  socket->user_cache[nick].nick = nick;
  socket->user_cache[nick].user = user;
  socket->user_cache[nick].host = host;
  if (message.size() > socket->prefix.size() && message.substr(0, socket->prefix.size()) == socket->prefix) {
    event_machine->fire(new PB::CommandEvent(socket, nick, user, host, target, message.substr(socket->prefix.size())));
  }
}

std::string PB::PRIVMSGEvent::to_s() {
  return
      "PRIVMSGEvent server=[" + socket->name
    + "] nick=[" + nick
    + "] user=[" + user
    + "] host=[" + host
    + "] target=[" + target
    + "] message=[" + message
    + "]";
}

PB::EventType PB::PRIVMSGEvent::type() {
  return PB::EventType::PRIVMSGEvent;
}

/*
 * NOTICE (NOTICEEvent)
 */

PB::NOTICEEvent::NOTICEEvent(IRCSocket* _socket, std::string _nick, std::string _user, std::string _host, std::string _target, std::string _message)
: socket(_socket)
, nick(_nick)
, user(_user)
, host(_host)
, target(_target)
, message(_message)
{}

void PB::NOTICEEvent::handler() {
  socket->user_cache[nick].nick = nick;
  socket->user_cache[nick].user = user;
  socket->user_cache[nick].host = host;
}

std::string PB::NOTICEEvent::to_s() {
  return
      "NOTICEEvent server=[" + socket->name
    + "] nick=[" + nick
    + "] user=[" + user
    + "] host=[" + host
    + "] target=[" + target
    + "] message=[" + message
    + "]";
}

PB::EventType PB::NOTICEEvent::type() {
  return PB::EventType::NOTICEEvent;
}

/*
 * Command (CommandEvent)
 */

template<typename Out>
static void split_str(const std::string &s, char delim, Out result) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

static std::vector<std::string> split_str(const std::string &s, char delim) {
  std::vector<std::string> e;
  split_str(s, delim, std::back_inserter(e));
  return e;
}

PB::CommandEvent::CommandEvent(IRCSocket* _socket, std::string _nick, std::string _user, std::string _host, std::string _target, std::string _message)
: socket(_socket)
, nick(_nick)
, user(_user)
, host(_host)
, target(_target)
, reply_to(_target == socket->nick ? _nick : _target)
, message(_message)
{}

void PB::CommandEvent::handler() {
  std::vector<std::string> s = split_str(message, ' ');
  command = s[0];
  for (auto it = s.begin() + 1; it != s.end(); ++it) {
    split.push_back(*it);
  }

  for (auto& p : bot->plugin_manager.plugins) {
    for (auto& g : (*p.second->commands)) {
      for (auto& c : g.second) {
        if (c.first == command) {
          int lvl = bot->get_level(socket->name, host);
          if (lvl >= c.second.level) {
            c.second.handler(this);
          } else {
            nreply("Error: your permission level is " + std::to_string(lvl) + ", while at least " + std::to_string(c.second.level) +" is required!");
          }
        }
      }
    }
  }
}

std::string PB::CommandEvent::to_s() {
  return
      "CommandEvent server=[" + socket->name
    + "] nick=[" + nick
    + "] user=[" + user
    + "] host=[" + host
    + "] target=[" + target
    + "] reply_to=[" + reply_to
    + "] message=[" + message
    + "]";
}

PB::EventType PB::CommandEvent::type() {
  return PB::EventType::CommandEvent;
}

void PB::CommandEvent::reply(std::string msg) {
  socket->privmsg(reply_to, msg);
}

void PB::CommandEvent::nreply(std::string msg) {
  socket->notice(nick, msg);
}