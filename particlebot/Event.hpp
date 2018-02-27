#pragma once

#ifdef PB_PLUGIN
  #define private public
  #define protected public
#else
  #define private private
  #define protected protected
#endif

namespace PB {

class Bot;
class EventMachine;
class IRCSocket;
class Plugin;
class CPPPlugin;

enum class EventType {
  Event,
  TestEvent,
  ConnectEvent,
  DisconnectEvent,
  IRCMessageEvent,
  CodeEvent,
  PingEvent,
  WHOREPLYEvent,
  JoinEvent,
  PartEvent,
  NickEvent,
  PRIVMSGEvent,
  NOTICEEvent,
  CommandEvent
};

class Event {
public:
  friend EventMachine;

  Event();
  virtual ~Event();
  virtual void handler() = 0;
  virtual std::string to_s() = 0;
  virtual EventType type() = 0;
   
  Guosh::Logger log;

protected:
  EventMachine* event_machine;
  Bot* bot;
};


class TestEvent : public Event {
public:
  TestEvent(std::string str);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::TestEvent; }

private:
  std::string str;
};


class ConnectEvent : public Event {
public:
  ConnectEvent(IRCSocket* socket);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::ConnectEvent; }

private:
  IRCSocket* socket;
};


class DisconnectEvent : public Event {
public:
  DisconnectEvent(IRCSocket* socket);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::DisconnectEvent; }

private:
  IRCSocket* socket;
};


class IRCMessageEvent : public Event {
public:
  IRCMessageEvent(IRCSocket* socket, std::string raw_msg);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::IRCMessageEvent; }

private:
  IRCSocket* socket;
  std::string raw_msg;
};


class CodeEvent : public Event {
public:
  CodeEvent(IRCSocket* socket, std::string code, std::string extra);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::CodeEvent; }

private:
  IRCSocket* socket;
  std::string code;
  std::string extra;
};


class PingEvent : public Event {
public:
  PingEvent(IRCSocket* socket, std::string target);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::PingEvent; }

private:
  IRCSocket* socket;
  std::string target;
};


class WHOREPLYEvent : public Event {
public:
  WHOREPLYEvent(IRCSocket* socket, std::string nick, std::string user, std::string host);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::WHOREPLYEvent; }

private:
  IRCSocket* socket;
  std::string nick;
  std::string user;
  std::string host;
};


class JoinEvent : public Event {
public:
  JoinEvent(IRCSocket* socket, std::string channel, std::string nick, std::string user, std::string host);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::JoinEvent; }

private:
  IRCSocket* socket;
  std::string channel;
  std::string nick;
  std::string user;
  std::string host;
};


class PartEvent : public Event {
public:
  PartEvent(IRCSocket* socket, std::string channel, std::string nick, std::string user, std::string host, std::string reason);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::PartEvent; }

private:
  IRCSocket* socket;
  std::string channel;
  std::string nick;
  std::string user;
  std::string host;
  std::string reason;
};


class NickEvent : public Event {
public:
  NickEvent(IRCSocket* socket, std::string nick, std::string user, std::string host, std::string new_nick);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::NickEvent; }

private:
  IRCSocket* socket;
  std::string nick;
  std::string user;
  std::string host;
  std::string new_nick;
};


class PRIVMSGEvent : public Event {
public:
  PRIVMSGEvent(IRCSocket* socket, std::string nick, std::string user, std::string host, std::string target, std::string message);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::PRIVMSGEvent; }

private:
  IRCSocket* socket;
  std::string nick;
  std::string user;
  std::string host;
  std::string target;
  std::string reply_to;
  std::string message;
};


class NOTICEEvent : public Event {
public:
  NOTICEEvent(IRCSocket* socket, std::string nick, std::string user, std::string host, std::string target, std::string message);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::PRIVMSGEvent; }

private:
  IRCSocket* socket;
  std::string nick;
  std::string user;
  std::string host;
  std::string target;
  std::string message;
};


class CommandEvent : public Event {
public:
  friend Plugin;
  friend CPPPlugin;
  
  CommandEvent(IRCSocket* socket, std::string nick, std::string user, std::string host, std::string target, std::string message);
  virtual void handler();
  virtual std::string to_s();
  virtual EventType type();
  static inline EventType type_static() { return EventType::CommandEvent; }

  void reply(std::string message);
  void nreply(std::string message);

private:
  IRCSocket* socket;
  std::string nick;
  std::string user;
  std::string host;
  std::string target;
  std::string reply_to;
  std::string message;
  std::string command;
  std::vector<std::string> split;
};


}