#pragma once

namespace PB {

class CommandEvent;
class EventMachine;
class Plugin;
class CPPPlugin;

class PluginManager {
public:
  friend CommandEvent;
  friend EventMachine;
  friend Plugin;
  friend CPPPlugin;

  PluginManager(Bot* bot);
  void load_cpp_plugin(std::string name);
  void unload_plugins();

private:
  Bot* bot;
  std::map<std::string, std::shared_ptr<Plugin>> plugins;
};

}