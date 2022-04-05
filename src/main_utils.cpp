#include "main_utils.hpp"

#include <netinet/in.h>
#include <signal.h>
#include <sys/event.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "Message.hpp"
#include "Server.hpp"

extern int g_sockfd;

void sig_handler(int sig_code) {
  if (sig_code == SIGINT) {
    if (g_sockfd > 0) {
      close(g_sockfd);
    }
    exit(0);
  }
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int setup_socket(int &option, struct sockaddr_in &serv_addr, struct sockaddr_in &cli_addr, int &clilen, int portno) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  if (sockfd < 0) {
    error("error opening socket");
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error("error on binding");
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  return sockfd;
}

void disconnection(struct kevent tevent, Server *server) {
  int temp_client_sockfd = static_cast<int>(tevent.ident);
  printf("Client has disconnected, sockfd = %d\n", temp_client_sockfd);
  close(tevent.ident);
  server->remove_client_sockfd(static_cast<int>(tevent.ident));
  Message::remove_connection_cache(temp_client_sockfd);
}

void show_main_usage() {
  std::cout << "Usage:" << std::endl;
  std::cout << "    ./ircserv <port> <server_password> [ path/to/words_list.txt ]" << std::endl;
  exit(1);
}

void check_main_arguments(int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    std::cout << "Incorrect number of arguments" << std::endl;
    show_main_usage();
  }
  for (std::size_t i = 0; argv[1][i]; i++) {
    if (!std::isdigit(argv[1][i])) {
      std::cout << "Argument <port> is not a number" << std::endl;
      show_main_usage();
    }
  }
  if (strlen(argv[2]) == 0) {
    std::cout << "Argument <server_password> is not long enough" << std::endl;
    show_main_usage();
  }
}
