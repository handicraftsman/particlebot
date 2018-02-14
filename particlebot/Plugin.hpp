#pragma once

namespace PB {

class Bot;
class CommandEvent;
class CommandInfo;
class EventMachine;
class PluginManager;

class Plugin {
public:
  friend CommandEvent;
  friend EventMachine;

  typedef std::map<PB::EventType, void (*)(PB::Event*) noexcept> EHMap;
  typedef std::map<std::string, std::map<std::string, CommandInfo>> CMap;

  Plugin(PB::PluginManager* plugin_manager, std::string name);
  ~Plugin();

private:
  std::string name;
  void* handle;
  PluginManager* plugin_manager;
  Guosh::Logger log;

  std::shared_ptr<EHMap> event_handlers;
  std::shared_ptr<CMap> commands;
};

}

#define pb_plugin_main() \
  extern "C" { \
    __attribute__((used)) std::string pb_init_error("ok"); \
    __attribute__((used)) std::map<PB::EventType, void (*)(PB::Event*) noexcept> pb_event_handlers; \
    __attribute__((used)) std::map<std::string, std::map<std::string, PB::CommandInfo>> pb_commands; \
    __attribute__((used)) PB::Plugin* pb_plugin; \
  }

#define pb_plugin() \
  extern "C" { \
    extern std::map<PB::EventType, void (*)(PB::Event*) noexcept> pb_event_handlers; \
    extern std::map<std::string, std::map<std::string, PB::CommandInfo>> pb_commands; \
    extern PB::Plugin* pb_plugin; \
  } \
  template<typename T> \
  inline void register_handler(void (*handler)(PB::Event*) noexcept) { \
    pb_event_handlers[T::type_static()] = handler; \
  } \
  inline PB::CommandInfo& register_command(std::string group, std::string cmd, void (*handler)(PB::CommandEvent*) noexcept) { \
    pb_commands[group][cmd] = PB::CommandInfo(handler); \
    return pb_commands[group][cmd]; \
  }
