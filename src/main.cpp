#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>

#include "IOManager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "main_utils.hpp"
#include "utils.hpp"

int g_sockfd = -1;

int main(int argc, char *argv[]) {
  signal(SIGINT, &sig_handler);
  struct kevent tevents[20];
  struct kevent tevent;
  std::vector<Message> parsed_message;
  int clilen, ret, option = 1, portno;
  std::string server_password;
  struct sockaddr_in serv_addr, cli_addr;

  check_main_arguments(argc, argv);
  portno = atoi(argv[1]);
  server_password = argv[2];
  g_sockfd = setup_socket(option, serv_addr, cli_addr, clilen, portno);
  IOManager io_manager(g_sockfd);
  Server server("LE_SERVER", "0.1", server_password, static_cast<IOManagerInterface *>(&io_manager));
  if (argc == 4 && !server.new_bot("bot", argv[3]))
    exit(1);
  while (1) {
    parsed_message.clear();
    ret = kevent(io_manager.get_kq(), 0, 0, tevents, 20, NULL);
    for (int i = 0; i < ret; i++) {
      tevent = tevents[i];
      if (tevent.flags & EV_EOF) {
        disconnection(tevent, &server);
      } else if (static_cast<int>(tevent.ident) == g_sockfd) {
        io_manager.new_connection(&server);
      } else if (tevent.filter == EVFILT_READ) {
        io_manager.read_event(&server, tevent);
      } else if (tevent.filter == EVFILT_WRITE) {
        io_manager.write_event(tevent);
      } else {
        std::cout << "Uncaught tevent\n";
      }
    }
  }
  return 0;
}
