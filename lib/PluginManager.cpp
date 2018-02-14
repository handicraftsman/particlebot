#include "include.hpp"

PB::PluginManager::PluginManager(PB::Bot* _bot)
: bot(_bot)
{
  load_plugin("core");
}

void PB::PluginManager::load_plugin(std::string name) {
  std::shared_ptr<PB::Plugin> plugin(new PB::Plugin(this, name));
  plugins[name] = plugin;
}

void PB::PluginManager::unload_plugins() {
  plugins.erase(plugins.begin(), plugins.end());
}