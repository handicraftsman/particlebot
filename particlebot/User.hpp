#pragma once

namespace PB {

class User {
public:
  User();

  std::optional<std::string> nick;
  std::optional<std::string> user;
  std::optional<std::string> host;
};

}