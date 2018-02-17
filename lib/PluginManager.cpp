#include "include.hpp"

PB::PluginManager::PluginManager(PB::Bot* _bot)
: bot(_bot)
{
  load_cpp_plugin("core");
}

void PB::PluginManager::load_cpp_plugin(std::string name) {
  std::shared_ptr<PB::Plugin> plugin(new PB::CPPPlugin(this, name));
  plugins[name] = plugin;
}

void PB::PluginManager::unload_plugins() {
  plugins.erase(plugins.begin(), plugins.end());
}