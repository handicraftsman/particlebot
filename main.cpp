#include "lib/include.hpp"

extern "C" {
  #include "getopt.h"
}

void set_log_level(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  Guosh::LogLevel lvl = Guosh::LogLevel::INFO;
  if (str == "debug") {
    lvl = Guosh::LogLevel::DEBUG;
  } else if (str == "io" || str == "irc") {
    lvl = Guosh::LogLevel::IO;
  } else if (str == "info") {
    lvl = Guosh::LogLevel::INFO;
  } else if (str == "warning") {
    lvl = Guosh::LogLevel::WARNING;
  } else if (str == "error") {
    lvl = Guosh::LogLevel::ERROR;
  } else if (str == "important") {
    lvl = Guosh::LogLevel::IMPORTANT;
  } else if (str == "critical") {
    lvl = Guosh::LogLevel::CRITICAL;
  } else {
    std::clog << "Invalid log level: " << str << std::endl;
    exit((int) Exitcodes::ProgramOptions);
  }
  Guosh::Logger::main_level = lvl;
}

void display_usage(char* pname) {
  std::cout << pname << " [OPTIONS]" << std::endl << std::endl;
  std::cout << "Available options: " << std::endl;
  std::cout << "-h        --help                Show this message" << std::endl;
  std::cout << "-c FILE   --config-file FILE    Config file [./particlebot.json]" << std::endl;
  std::cout << "-b FILE   --database-file FILE  Database file [./particlebot.db]" << std::endl;
  std::cout << "-l LEVEL  --log-level LEVEL     Log level [info]" << std::endl;
  std::cout << "Possible log levels: debug, [io | irc], info, warning, error, important, critical" << std::endl;
}

int main(int argc, char** argv) {
  int opt;
  
  static struct option long_options[] = {
    {"help", 0, NULL, 'h'},
    {"config-file", required_argument, NULL, 'c'},
    {"database-file", required_argument, NULL, 'd'},
    {"log-level", required_argument, NULL, 'l'},
  };

  std::string config_file = "./particlebot.json";
  std::string db_file = "./particlebot.db";

  while ((opt = getopt_long(argc,argv,"hvc:d:l:", long_options, NULL)) != EOF)
    switch(opt) {
    case 'c':
      config_file = std::string(optarg);
      break;
    case 'd':
      db_file = std::string(optarg);
      break;
    case 'l':
      set_log_level(std::string(optarg));
      break;
    case 'h':
      display_usage(argv[0]);
      exit((int) Exitcodes::ProgramOptions);
    default:
      exit((int) Exitcodes::ProgramOptions);
  }

  PB::Bot bot(config_file, Guosh::LogLevel::INFO, db_file);

  return (int) Exitcodes::Success;
}