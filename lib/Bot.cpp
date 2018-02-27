#include "include.hpp"

#include <config.hpp>

std::string PB::version = PB_VERSION;

PB::Bot::Bot(std::string& config_file, Guosh::LogLevel lvl, std::string& db_file)
: prefix("!")
, plugin_manager(this)
, log("ParticleBot", lvl)
{
  log.write("Hello, world!");

  if (sqlite3_open(db_file.c_str(), &db)) {
    log.error("Error: %s", sqlite3_errmsg(db));
    throw std::runtime_error("Unable to initialize the database");
  }

  sqlite3_stmt* perms_stmt;
  sqlite3_prepare(
    db,
    "CREATE TABLE IF NOT EXISTS perms (" \
      "server varchar(256)," \
      "host varchar(256)," \
      "level integer" \
    ");",
    -1,
    &perms_stmt,
    NULL
  );
  if (sqlite3_step(perms_stmt) != SQLITE_DONE) {
    log.error("Error: %s", sqlite3_errmsg(db));
    throw std::runtime_error("Unable to initialize the database");
  }
  sqlite3_finalize(perms_stmt);

  event_machine.bot = this;

  parse_config(config_file);

  for (auto p : sockets) {
    std::thread([p] () {
      p.second->connect();
    }).detach();
  }
  
  event_machine.loop().join();
}

void PB::Bot::parse_config(std::string& config_file) {
  log.write("Parsing config file... (%s)", config_file.c_str());
  
  Json::Value root;  
  std::ifstream f(config_file);
  f >> root;

  if (!root.isObject())
    throw std::runtime_error("config root is not an object");

  if (!root["prefix"].isNull())
    prefix = root["prefix"].asString();
  log.write("Prefix: %s", prefix.c_str());

  if (!root["servers"].isObject())
    throw std::runtime_error("config root[\"servers\"] is not an object");

  if (!root["plugins"].isArray() && !root["plugins"].isNull())
      throw std::runtime_error("config root[\"plugins\"] is not an array");

  if (!root["lua-plugins"].isArray() && !root["lua-plugins"].isNull())
      throw std::runtime_error("config root[\"lua-plugins\"] is not an array");

  for (Json::Value v : root["plugins"])
    plugin_manager.load_cpp_plugin(v.asString());

  for (Json::Value v : root["lua-plugins"])
    plugin_manager.load_lua_plugin(v.asString());

  for (auto it = root["servers"].begin(); it != root["servers"].end(); ++it) {
    std::string name = it.key().asString();
    sockets[name] = std::shared_ptr<PB::IRCSocket>(new PB::IRCSocket(this, name));

    Json::Value& srv = *it;

    sockets[name]->host = srv["host"].asString();

    if (srv["port"].isNull()) {
      sockets[name]->port = 6667;
    } else {
      sockets[name]->port = srv["port"].asUInt();
    }

    if (srv["nick"].isNull()) {
      sockets[name]->nick = "ParticleBot";
    } else {
      sockets[name]->nick = srv["nick"].asString();
    }

    if (srv["user"].isNull()) {
      sockets[name]->user = sockets[name]->nick;
    } else {
      sockets[name]->user = srv["user"].asString();
    }

    if (srv["rnam"].isNull()) {
      sockets[name]->rnam = "An IRC bot in C++";
    } else {
      sockets[name]->rnam = srv["rnam"].asString();
    }

    if (srv["pass"].isNull()) {
      sockets[name]->has_pass = false;
    } else {
      sockets[name]->has_pass = true;
      sockets[name]->pass = srv["pass"].asString();
    }

    if (!srv["autojoin"].isNull()) {
      for (auto chan : srv["autojoin"]) {
        sockets[name]->autojoin.push_back(chan.asString());
      }
    }

    if (srv["prefix"].isNull()) {
      sockets[name]->prefix = prefix;
    } else {
      sockets[name]->prefix = srv["prefix"].asString();
    }

    if (!srv["autoowner"].isNull()) {
      sockets[name]->autoowner = srv["autoowner"].asString();
    }

    sockets[name]->print_info();
  }
}

int PB::Bot::get_level(std::string server, std::string host) {
  sqlite3_stmt* level_stmt;
  sqlite3_prepare(
    db,
    "SELECT level FROM perms WHERE server=? AND host=?;",
    -1,
    &level_stmt,
    NULL
  );
  sqlite3_bind_text(level_stmt, 1, server.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(level_stmt, 2, host.c_str(), -1, SQLITE_TRANSIENT);
  int r = sqlite3_step(level_stmt);
  if (r == SQLITE_DONE) {
    sqlite3_finalize(level_stmt);
    return 0;
  } else {
    int lvl = sqlite3_column_int(level_stmt, 0);
    sqlite3_finalize(level_stmt);
    return lvl;
  }
  return 0;
}

void PB::Bot::set_level(std::string server, std::string host, int level) {
  if (has_level(server, host)) {
    sqlite3_stmt* level_stmt;
    sqlite3_prepare(
      db,
      "UPDATE perms SET level=? where server=? AND host=?;",
      -1,
      &level_stmt,
      NULL
    );
    sqlite3_bind_int(level_stmt, 1, level);
    sqlite3_bind_text(level_stmt, 2, server.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(level_stmt, 3, host.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(level_stmt);
  } else {
    sqlite3_stmt* level_stmt;
    sqlite3_prepare(
      db,
      "INSERT INTO perms (server, host, level) VALUES (?, ?, ?);",
      -1,
      &level_stmt,
      NULL
    );
    sqlite3_bind_text(level_stmt, 1, server.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(level_stmt, 2, host.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(level_stmt, 3, level);
    sqlite3_step(level_stmt);
  }
}

bool PB::Bot::has_level(std::string server, std::string host) {
  sqlite3_stmt* level_stmt;
  sqlite3_prepare(
    db,
    "SELECT level FROM perms WHERE server=? AND host=?;",
    -1,
    &level_stmt,
    NULL
  );
  sqlite3_bind_text(level_stmt, 1, server.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(level_stmt, 2, host.c_str(), -1, SQLITE_TRANSIENT);
  int r = sqlite3_step(level_stmt);
  if (r == SQLITE_DONE) {
    return false;
  } else {
    return true;
  }
  return 0;
}