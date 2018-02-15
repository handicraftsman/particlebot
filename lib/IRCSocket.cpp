#include "include.hpp"

PB::IRCSocket::IRCSocket(PB::Bot* _bot, std::string _name)
: name(_name)
, nick("ParticleBot")
, user("ParticleBot")
, rnam("An IRC bot in C++")
, has_pass(false)
, prefix("!")
, reconnects(0)
, running(false)
, bot(_bot)
, conn(0)
, burst(0)
, log("!" + name, Guosh::LogLevel::IO)
{
  log.iochars = "IRC";
  log.info("Hello, %s!", _name.c_str());
}

PB::IRCSocket::~IRCSocket() {
  close(conn);
}

void PB::IRCSocket::connect() {
  log.important("Connecting...");
  
  running = true;

  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) < 0) {
    log.error("Cannot resolve %s:%d", host.c_str(), port);
    handle_disconnect();
    return;
  }

  conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (conn == -1) {
    log.error("Cannot create a socket");
    handle_disconnect();
    return;
  }
  if (::connect(conn, res->ai_addr, res->ai_addrlen) == -1) {
    log.error("Cannot connect to %s:%d", host.c_str(), port);
    handle_disconnect();
    return;
  }

  freeaddrinfo(res);

  reconnects = 0;

  log.important("Connected");

  if (has_pass) raw_write("PASS " + pass);
  raw_write("NICK " + nick);
  raw_write("USER " + user + " 0 * :" + rnam);

  std::thread([&, this] () { read_loop(); }).detach();
  std::thread([&, this] () { write_loop(); }).detach();

  if (autoowner != std::nullopt) {
    bot->set_level(name, autoowner.value_or("<UNDEFINED>"), 4);
  }
}

void PB::IRCSocket::handle_disconnect() {
  std::lock_guard<std::mutex> lock(running_mtx);
  if (running == true) {
    running = false;
    bot->event_machine.fire(new PB::DisconnectEvent(this));
  }
}

bool PB::IRCSocket::disconnected() {
  return fcntl(conn, F_GETFL) < 0 && errno == EBADF;
}

void PB::IRCSocket::raw_write(std::string msg) {
  if (disconnected()) {
    handle_disconnect();
    return;
  }
  ::write(conn, (msg + "\r\n").c_str(), msg.size() + 2);
  log.io("W> " + msg);
}

void PB::IRCSocket::write(std::string msg) {
  write_queue.push(msg);
}

void PB::IRCSocket::join(std::string chan) {
  write("JOIN " + chan);
}

void PB::IRCSocket::part(std::string chan, std::string reason) {
  write("PART " + chan + " :" + reason);
}

void PB::IRCSocket::change_nick(std::string new_nick) {
  write("NICK " + new_nick);
}

void PB::IRCSocket::who(std::string mask) {
  write("WHO " + mask);
}

static std::vector<std::string> split_every(std::string& s, size_t e) {
  std::vector<std::string> o;
  std::string b;

  size_t i = 0;
  for (char c : s) {
    if (i == e) {
      o.push_back(b);
      b = "";
      i = 0;
    }
    b += c;
    ++i;
  }
  
  if (!b.empty())
    o.push_back(b);

  return o;
}

static void replace_all(std::string& str, const std::string& from, const std::string& to) {
  if(from.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

static void format(std::string& message) {
  static std::vector<std::pair<std::string, std::string>> rules {
    std::pair<std::string, std::string>("&N",      "\x0F"),
    std::pair<std::string, std::string>("&B",      "\x02"),
    std::pair<std::string, std::string>("&U",      "\x1F"),
    std::pair<std::string, std::string>("&I",      "\x10"),
    std::pair<std::string, std::string>("%C",      "\x03"),
    std::pair<std::string, std::string>("?WHITE",  "0"),
    std::pair<std::string, std::string>("?BLACK",  "1"),
    std::pair<std::string, std::string>("?BLUE",   "2"),
    std::pair<std::string, std::string>("?GREEN",  "3"),
    std::pair<std::string, std::string>("?RED",    "4"),
    std::pair<std::string, std::string>("?BROWN",  "5"),
    std::pair<std::string, std::string>("?PURPLE", "6"),
    std::pair<std::string, std::string>("?ORANGE", "7"),
    std::pair<std::string, std::string>("?YELLOW", "8"),
    std::pair<std::string, std::string>("?LGREEN", "9"),
    std::pair<std::string, std::string>("?CYAN",   "10"),
    std::pair<std::string, std::string>("?LCYAN",  "11"),
    std::pair<std::string, std::string>("?LBLUE",  "12"),
    std::pair<std::string, std::string>("?PINK",   "13"),
    std::pair<std::string, std::string>("?GREY",   "14"),
    std::pair<std::string, std::string>("?LGREY",  "15")
  };
  for (auto p : rules) {
    replace_all(message, p.first, p.second);
  }
}

void PB::IRCSocket::privmsg(std::string target, std::string message) {
  format(message);
  for (std::string str : split_every(message, 400)) {
    write("PRIVMSG " + target + " :" + str);
  }
}

void PB::IRCSocket::ctcp(std::string target, std::string message) {
  format(message);
  write("PRIVMSG " + target + " :\x01" + message + "\x01");
}

void PB::IRCSocket::notice(std::string target, std::string message) {
  format(message);
  for (std::string str : split_every(message, 400)) {
    write("NOTICE " + target + " :" + str);
  }
}

void PB::IRCSocket::nctcp(std::string target, std::string message) {
  format(message);
  write("NOTICE " + target + " :\x01" + message + "\x01"); 
}

void PB::IRCSocket::mode(std::string channel, std::string args) {
  write("MODE " + channel + " " + args);
}

void PB::IRCSocket::kick(std::string channel, std::string target, std::string reason) {
  write("KICK " + channel + " " + target + " :" + reason);
}

void PB::IRCSocket::remove(std::string channel, std::string target, std::string reason) {
  write("REMOVE " + channel + " " + target + " :" + reason);
}

void PB::IRCSocket::print_info() {
  log.info("Server: %s", name.c_str()); 
  log.info("Addr: %s:%d", host.c_str(), port);
  log.info("Nick: %s", nick.c_str());
  log.info("User: %s", user.c_str());
  log.info("Real Name: %s", rnam.c_str());
  if (has_pass)
    log.info("Password: %s", pass.c_str());
}

static int read_line(int fd, char* buf) {
  memset(buf, 0, 513);
  int i = 0;
  while (i < 512) {
    int ok = read(fd, &buf[i], 1);
    if (ok < 1) {
      return -1;
    }
    if (i > 0 && buf[i-1] == '\r' && buf[i] == '\n') {
      return 0;
    }
    ++i;
  }
  return -2;
}

void PB::IRCSocket::read_loop() {
  while (running) {
    if (disconnected()) {
      handle_disconnect();
      return;
    }
    char buf[513];
    int ok = read_line(conn, buf);
    if (ok < 0) {
      handle_disconnect();
      return;
    }
    std::string msg(buf);
    msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
    msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
    bot->event_machine.fire(new IRCMessageEvent(this, msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void PB::IRCSocket::write_loop() {
  while (running) {
    if (burst >= 5) {
      std::this_thread::sleep_for(std::chrono::milliseconds(700));
    } else {
      ++burst;
    }
    auto current_time = std::chrono::system_clock::now();
    if (current_time - last_write >= std::chrono::milliseconds(700)) {
      burst = 0;
    }
    if (!write_queue.empty()) {
      std::string msg = write_queue.front();
      write_queue.pop();
      raw_write(msg);
      last_write = current_time;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}