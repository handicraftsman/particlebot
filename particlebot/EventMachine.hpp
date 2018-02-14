#pragma once

namespace PB {

class Bot;
class Event;

class EventMachine {
public:
  friend Bot;

  EventMachine();
  std::thread loop();
  void fire(Event* event);
  
protected:
  Bot* bot;
  std::queue<std::shared_ptr<Event>> queue;
  std::mutex queue_mutex;

private:
  bool running;
  Guosh::Logger log;
};

}