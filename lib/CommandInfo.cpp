#include "include.hpp"

PB::CommandInfo::CommandInfo(void (*_handler)(PB::CommandEvent*) noexcept, std::string _description)
: description(_description)
, usage("[OPTIONS]")
, level(0)
, cooldown(0)
, handler(_handler)
{}

PB::CommandInfo::CommandInfo() 
: description("No description")
, usage("[OPTIONS]")
, level(0)
, cooldown(0)
, handler(NULL)
{}