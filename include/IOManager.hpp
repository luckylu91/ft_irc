#pragma once

#include <sys/event.h>

#include <map>

#include "IOManagerInterface.hpp"
#include "Server.hpp"

class IOManager : public IOManagerInterface {
 private:
  std::map<int, std::string> task_tree;
  int server_fd;
  struct kevent triggers;
  int kq;
  int clilen;

 public:
  IOManager(int _server_fd) : server_fd(_server_fd) {
    kq = kqueue();
    EV_SET(&triggers, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    kevent(kq, &triggers, 1, NULL, 0, NULL);
  }
  virtual ~IOManager() {}
  void new_connection(Server* server);
  void read_event(Server* server, struct kevent tevent);
  void write_event(struct kevent tevent);
  virtual void send_message(int client_sockfd, Message const& message);
  int get_kq() const { return kq; }
};
