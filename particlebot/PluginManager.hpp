#pragma once

namespace PB {

class CommandEvent;
class EventMachine;
class Plugin;

class PluginManager {
public:
  friend CommandEvent;
  friend EventMachine;
  friend Plugin;

  PluginManager(Bot* bot);
  void load_plugin(std::string name);
  void unload_plugins();

private:
  Bot* bot;
  std::map<std::string, std::shared_ptr<Plugin>> plugins;
};

}