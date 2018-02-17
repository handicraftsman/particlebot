#pragma once

namespace PB {

class CPPPlugin;
class CommandInfo;

class Plugin {
public:
  friend CPPPlugin;

  typedef std::map<std::string, std::map<std::string, CommandInfo>> CMap;

  enum class Type {
    None,
    CPP,
    Lua
  };

  Plugin(PB::PluginManager* plugin_manager, std::string name, Type type);
  virtual ~Plugin();

  virtual void handle_event(std::shared_ptr<Event> e) = 0;
  bool handle_command(CommandEvent* e);
  
  std::string name;
  Type type;

protected:
  PluginManager* plugin_manager;
  std::shared_ptr<CMap> commands;
  Guosh::Logger log;
};

}