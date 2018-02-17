#include "include.hpp"

PB::EventMachine::EventMachine()
: running(false)
, log("EventMachine")
{
  log.write("Hello, event queue!");
  fire(new PB::TestEvent("Test Event"));
}

void PB::EventMachine::fire(PB::Event* event) {
  std::lock_guard<std::mutex> lock(queue_mutex);
  event->event_machine = this;
  event->bot = this->bot;
  queue.push(std::shared_ptr<PB::Event>(event));
}

std::thread PB::EventMachine::loop() {
  log.write("Starting event loop...");
  running = true;
  return std::thread([this] () {
    while (running) {
      if (!queue.empty()){
        std::lock_guard<std::mutex> lock(queue_mutex);
        std::shared_ptr<PB::Event> e = queue.front();
        std::thread([this, e] () {
          try {
            e->handler();
          } catch (std::exception& err) {
            e->log.error("Error in %s", e->to_s());
            e->log.error("] %s", err.what());
          }
          for (auto p : bot->plugin_manager.plugins) {
            p.second->handle_event(e);
            /*
            if (p.second->event_handlers->find(e->type()) != p.second->event_handlers->end()) {
              std::thread([p, e] () {
                (*p.second->event_handlers)[e->type()](e.get());
              }).detach();
            }
            */
          }
        }).detach();
        queue.pop();
        continue;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  });
}