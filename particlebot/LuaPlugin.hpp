#pragma once

namespace PB {

class LuaPlugin : public Plugin {
public:
  LuaPlugin(PB::PluginManager* plugin_manager, std::string name);
  virtual ~LuaPlugin();

  virtual void handle_event(std::shared_ptr<Event> e);
  void load();
  void unload();

private:
  lua_State* L;
  std::mutex lmtx;
};

}