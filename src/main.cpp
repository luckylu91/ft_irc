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
#include "io_manager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "main_utils.hpp"
#include "utils.hpp"

int sockfd = -1;
//
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

void	disconnection(struct kevent tevent, Server * server)
{
	int temp_client_sockfd = static_cast<int>(tevent.ident);
	printf("Client has disconnected, sockfd = %d\n",temp_client_sockfd);
	close(tevent.ident);
	server->remove_client_sockfd(static_cast<int>(tevent.ident));
	Message::remove_connection_cache(sockfd);
}

int main(int, char *argv[]) {
	signal(SIGINT, &sig_handler);
	struct kevent tevents[20];
	struct kevent tevent;
	std::vector<Message> parsed_message;
	int clilen, ret, option = 1;
	struct sockaddr_in serv_addr, cli_addr;

	if (setup_socket(option, serv_addr, cli_addr, clilen, argv))
		return 1;
	Io_manager io_manager(sockfd, clilen);
	Server server("LE_SERVER", "0.1", "root",&io_manager);
	server.new_bot("bbot", "liste_mots.txt");
	
	while (1) {
		parsed_message.clear();
		ret = kevent(io_manager.get_kq(), 0, 0, tevents, 20, NULL);
		for (int i = 0; i < ret; i++) {
			tevent = tevents[i];
			if (tevent.flags & EV_EOF) {
				disconnection(tevent, &server);
			}
			else if (static_cast<int>(tevent.ident) == sockfd) {
				io_manager.new_connection(&server);
			}
			else if (tevent.filter == EVFILT_READ){
				io_manager.incoming_message(&server,tevent);
			}
			else if (tevent.filter == EVFILT_WRITE)
			{
				io_manager.sending_message(tevent);
			}
			else
			{
				std::cout<<"Uncaught tevent\n";
			}
		}
	}
	return 0;
}
