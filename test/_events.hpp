#pragma once

#include <string>
#include "_MockIOManager.hpp"

struct IEvent {
  virtual void register_event(MockIOManager & io_manager) = 0;
};

struct ConnectionEvent: IEvent {
  int sockfd;
  virtual void register_event(Server * server, MockIOManager * io_manager) {
    io_manager->new_connection(server, sockfd);
  }
};

struct ReadEvent: IEvent {
  int sockfd;
  std::string message;
  virtual void register_event(Server * server, MockIOManager * io_manager) {
    io_manager->read_string(server, sockfd, message);
  }
};
