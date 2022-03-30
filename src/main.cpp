#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Message.hpp"
#include "Server.hpp"
#include "main_utils.hpp"
#include "utils.hpp"

int sockfd = -1;

int setup_socket(int &option, struct sockaddr_in &serv_addr, struct sockaddr_in &cli_addr, int &clilen, char *argv[]) {
  int portno;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  if (sockfd < 0) {
    error("error opening socket");
    return 1;
  }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error("error on binding");
    return 1;
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  return 0;
}

int main(int, char *argv[]) {
  signal(SIGINT, &sig_handler);
  struct kevent event; /* Event we want to monitor */
  struct kevent tevents[20];
  struct kevent tevent;
  std::vector<Message> parsed_message;
  int clilen, n, kq, ret, client_sockfd, option = 1;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;

  if (setup_socket(option, serv_addr, cli_addr, clilen, argv))
    return 1;
  Server server("LE_SERVER", "0.1", "root");
  server.new_bot("bbot", "liste_mots.txt");
  kq = kqueue();
  EV_SET(&event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  ret = kevent(kq, &event, 1, NULL, 0, NULL);

  while (1) {
    parsed_message.clear();
    ret = kevent(kq, 0, 0, tevents, 20, NULL);
    for (int i = 0; i < ret; i++) {
      tevent = tevents[i];
      if (tevent.flags & EV_EOF) {
        client_sockfd = static_cast<int>(tevent.ident);
        printf("Client has disconnected, sockfd = %d\n", client_sockfd);
        close(tevent.ident);
        server.remove_client_sockfd(static_cast<int>(tevent.ident));
        Message::remove_connection_cache(sockfd);
      }
      else if (static_cast<int>(tevent.ident) == sockfd) {
        client_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        printf("New connection, sockfd = %d\n", client_sockfd);
        server.new_client(client_sockfd, cli_addr);
        EV_SET(&event, client_sockfd, EVFILT_READ, EV_ADD, 0, 0, 0);
        ret = kevent(kq, &event, 1, NULL, 0, NULL);
      }
      else {
        client_sockfd = static_cast<int>(tevent.ident);
        bzero(buffer, 256);
        n = recv(tevent.ident, buffer, 255, 0);
        if (n < 0)
          error("ERROR reading from socket");
        client_sockfd = static_cast<int>(tevent.ident);
        Message::parse(buffer, &parsed_message, client_sockfd);
        for (std::vector<Message>::iterator it = parsed_message.begin(); it != parsed_message.end(); it++)
          server.receive_message(client_sockfd, *it);
      }
    }
  }
  return 0;
}
