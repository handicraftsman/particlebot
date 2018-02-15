#pragma once

namespace PB {

class CommandEvent;

// implement this
class CommandInfo {
public:
  CommandInfo(void (*handler)(PB::CommandEvent*) noexcept, std::string description = "No description");
  CommandInfo();

  std::string description;
  std::string usage;
  int level;
  int cooldown;
  void (*handler)(PB::CommandEvent*) noexcept;
  std::map<std::string, std::map<std::string, std::map<std::string, std::chrono::time_point<std::chrono::system_clock>>>> last_uses;
};

}