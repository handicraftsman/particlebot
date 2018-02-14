#pragma once

namespace PB {

class CommandEvent;
class EventMachine;
class IRCSocket;
class PluginManager;

class Bot {
public:
  friend CommandEvent;
  friend EventMachine;
  friend IRCSocket;

  Bot(std::string& config_file, Guosh::LogLevel lvl, std::string& db_file);

  std::string prefix;

  std::map<std::string, std::shared_ptr<IRCSocket>> sockets;

  int get_level(std::string server, std::string host);
  void set_level(std::string server, std::string host, int level);
  bool has_level(std::string server, std::string host);

private:
  void parse_config(std::string& config_file);
  
  EventMachine event_machine;
  PluginManager plugin_manager;
  Guosh::Logger log;
  sqlite3* db;
};

}