#include "IOManager.hpp"

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

#include <map>

#include "Message.hpp"
#include "Server.hpp"
#include "main_utils.hpp"
#include "utils.hpp"

void IOManager::new_connection(Server *server) {
  struct sockaddr_in cli_addr;
  int client_sockfd = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
  printf("New connection, sockfd = %d\n", client_sockfd);
  server->new_client(client_sockfd, cli_addr);
  EV_SET(&triggers, client_sockfd, EVFILT_READ, EV_ADD, 0, 0, 0);
  kevent(kq, &triggers, 1, NULL, 0, NULL);
}

void IOManager::read_event(Server *server, struct kevent tevent) {
  char buffer[256];
  std::vector<Message> parsed_message;
  int n;
  int client_sockfd = static_cast<int>(tevent.ident);

  bzero(buffer, 256);
  n = recv(tevent.ident, buffer, 255, 0);
  if (n < 0)
    error("ERROR reading from socket");
  client_sockfd = static_cast<int>(tevent.ident);
  Message::parse(buffer, &parsed_message, client_sockfd);
  for (std::vector<Message>::iterator it = parsed_message.begin(); it != parsed_message.end(); it++)
    server->receive_message(client_sockfd, *it);
}

void IOManager::send_message(int client_sockfd, Message const &message) {
  std::map<int, std::string>::iterator it = task_tree.find(client_sockfd);
  if (it == task_tree.end()) {
    task_tree.insert(std::make_pair(client_sockfd, message.to_string()));
    EV_SET(&triggers, client_sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
    kevent(kq, &triggers, 1, NULL, 0, NULL);
  } else {
    (it->second).append(message.to_string());
  }
}

void IOManager::write_event(struct kevent tevent) {
  int n;
  std::map<int, std::string>::iterator it = task_tree.find(tevent.ident);
  if (it == task_tree.end()) {
    std::cout << "Absence du fd=" << tevent.ident << " dans l'arbre\n";
    return;
  }
  std::string string_to_send = it->second;
  if (((it->second)).empty()) {
    std::cout << "chaine vide dans sending message\n";
    return;
  }
  if ((size_t)tevent.data < string_to_send.size()) {
    n = write(tevent.ident, ((it->second).substr(0, (size_t)tevent.data)).c_str(), tevent.data);
    if (n < 0) {
      std::cout << "Erreur write fd = " << tevent.ident << std::endl;
      return;
    }
    (it->second).erase(0, (size_t)tevent.data);
  } else {
    n = write(tevent.ident, (it->second).c_str(), (it->second).size());
    if (n < 0) {
      std::cout << "Erreur write fd = " << tevent.ident << std::endl;
      return;
    }
    if ((unsigned long)n < (it->second).size()) {
      (it->second).erase(0, (it->second).size());
      return;
    }
    task_tree.erase(tevent.ident);
    EV_SET(&triggers, tevent.ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
    kevent(kq, &triggers, 1, NULL, 0, NULL);
  }
}
