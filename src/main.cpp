#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <unistd.h>
#include <sys/socket.h>

#include "IOManager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "main_utils.hpp"
#include "utils.hpp"

int g_sockfd = -1;

int main(int, char *argv[]) {
	signal(SIGINT, &sig_handler);
	struct kevent tevents[20];
	struct kevent tevent;
	std::vector<Message> parsed_message;
	int clilen, ret, option = 1;
	struct sockaddr_in serv_addr, cli_addr;

	g_sockfd = setup_socket(option, serv_addr, cli_addr, clilen, atoi(argv[1]));
	IOManager io_manager(g_sockfd);
	Server server("LE_SERVER", "0.1", "root", static_cast<IOManagerInterface*>(&io_manager));
	server.new_bot("bbot", "liste_mots.txt");
	while (1) {
		parsed_message.clear();
		ret = kevent(io_manager.get_kq(), 0, 0, tevents, 20, NULL);
		for (int i = 0; i < ret; i++) {
			tevent = tevents[i];
			if (tevent.flags & EV_EOF) {
				disconnection(tevent, &server);
			}
			else if (static_cast<int>(tevent.ident) == g_sockfd) {
				io_manager.new_connection(&server);
			}
			else if (tevent.filter == EVFILT_READ){
				io_manager.read_event(&server,tevent);
			}
			else if (tevent.filter == EVFILT_WRITE) {
				io_manager.write_event(tevent);
			}
			// else
			// {
			// 	std::cout<<"ca boucle ?\n";
			// }
		}
	}
	return 0;
}
