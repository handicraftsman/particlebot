#pragma once

namespace PB {

class Bot;
class IRCMessageEvent;
class WHOREPLYEvent;

class IRCSocket {
public:
  friend Bot;
  friend WHOREPLYEvent;

  IRCSocket(Bot* bot, std::string name);
  ~IRCSocket();
  void connect();
  void handle_disconnect();
  bool disconnected();

  void raw_write(std::string msg);
  void write(std::string msg);

  void join(std::string chan);
  void part(std::string chan, std::string reason = "Bye");

  void who(std::string mask);

  void privmsg(std::string target, std::string message);
  void ctcp(std::string target, std::string message);
  void notice(std::string target, std::string message);
  void nctcp(std::string target, std::string message);

  void mode(std::string channel, std::string args);
  void kick(std::string channel, std::string target, std::string reason = "Bye");
  void remove(std::string channel, std::string target, std::string reason = "Bye");
  
  inline void op(std::string channel, std::string target) {
    mode(channel, "+o " + target);
  }

  inline void deop(std::string channel, std::string target) {
    mode(channel, "-o " + target);
  }

  inline void hop(std::string channel, std::string target) {
    mode(channel, "+h " + target);
  }

  inline void dehop(std::string channel, std::string target) {
    mode(channel, "-h " + target);
  }

  inline void voice(std::string channel, std::string target) {
    mode(channel, "+v " + target);
  }

  inline void devoice(std::string channel, std::string target) {
    mode(channel, "-v " + target);
  }

  inline void quiet(std::string channel, std::string target) {
    mode(channel, "+q " + target);
  }

  inline void unquiet(std::string channel, std::string target) {
    mode(channel, "-q " + target);
  }

  inline void ban(std::string channel, std::string target) {
    mode(channel, "+b " + user_cache[target].host.value_or(target));
  }

  inline void unban(std::string channel, std::string target) {
    mode(channel, "-b " + user_cache[target].host.value_or(target));
  }

  inline void exempt(std::string channel, std::string target) {
    mode(channel, "+e " + user_cache[target].host.value_or(target));
  }

  inline void unexempt(std::string channel, std::string target) {
    mode(channel, "-e " + user_cache[target].host.value_or(target));
  }

  void print_info();

  std::string name;
  std::string host;
  uint16_t port;
  std::string nick;
  std::string user;
  std::string rnam;
  std::string pass;
  bool has_pass;
  std::vector<std::string> autojoin;
  std::optional<std::string> autoowner;

  std::string prefix;

  int reconnects;
  
  std::map<std::string, User> user_cache;

private:
  void read_loop();
  void write_loop();

  bool running;
  std::mutex running_mtx;
  
  Bot* bot;
  int conn;

  std::queue<std::string> write_queue;
  uint16_t burst;
  std::chrono::time_point<std::chrono::system_clock> last_write;

public:
  Guosh::Logger log;
};

}