#include "include.hpp"

static std::string find_path_to_plugin(std::string name) {
  const std::vector<std::string> paths {
    "./libpb-" + name + ".so",
    "/usr/share/libpb-" + name + ".so",
    "/usr/local/share/libpb-" + name + ".so"
  };

  for (std::string path : paths)
    if (std::ifstream(path).good())
      return path;

  throw std::runtime_error("Unable to find plugin " + name);
}

PB::Plugin::Plugin(PB::PluginManager* _plugin_manager, std::string _name)
: name(_name)
, plugin_manager(_plugin_manager)
, log("?" + _name)
{
  log("Hello, %s!", name.c_str());

  char* error;

  std::string path = find_path_to_plugin(name);

  handle = dlopen(path.c_str(), RTLD_LAZY);
  if (!handle) {
    throw std::runtime_error(std::string(dlerror()));
  }
  dlerror();

  PB::Plugin** p = (PB::Plugin**) dlsym(handle, "pb_plugin");
  if ((error = dlerror()) != NULL) {
    throw std::runtime_error(std::string(error));
  }
  *p = this;

  event_handlers = std::shared_ptr<PB::Plugin::EHMap>((PB::Plugin::EHMap*) dlsym(handle, "pb_event_handlers"));
  if ((error = dlerror()) != NULL) {
    throw std::runtime_error(std::string(error));
  }

  commands = std::shared_ptr<PB::Plugin::CMap>((PB::Plugin::CMap*) dlsym(handle, "pb_commands"));
  if ((error = dlerror()) != NULL) {
    throw std::runtime_error(std::string(error));
  }

  bool (*init_func)(PB::Bot*) noexcept = (bool (*)(PB::Bot*) noexcept) dlsym(handle, "pb_init");
  if ((error = dlerror()) != NULL)
    throw std::runtime_error(std::string(error));
  if (!init_func(plugin_manager->bot)) {
    std::string* init_error = (std::string*) dlsym(handle, "pb_init_error");
    if ((error = dlerror()) != NULL) {
      throw std::runtime_error("cannot initialize plugin `" + name + "` and cannot get the error message");
    } else {
      throw std::runtime_error("cannot initialize plugin `" + name + "`: " + (*init_error));
    }
  }
}

PB::Plugin::~Plugin() {
  dlclose(handle);
}