#include "main_utils.hpp"

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "Message.hpp"

extern int sockfd;

void sig_handler(int sig_code) {
  if (sig_code == SIGINT) {
    if (sockfd > 0) {
      close(sockfd);
    }
    exit(0);
  }
}

void error(const char *msg) {
  perror(msg);
  exit(0);
}
