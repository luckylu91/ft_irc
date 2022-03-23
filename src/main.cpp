#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/event.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "main_utils.hpp"
#include "utils.hpp"
#include "Message.hpp"
#include "Server.hpp"

int sockfd = -1;

int main(int, char *argv[])
{
	struct	kevent event;	 /* Event we want to monitor */
	struct	kevent tevents[20];

	struct	kevent tevent;
	std::vector<Message> vec;
	int portno, clilen, n, kq, ret;

	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	Server server("LE_SERVER", "0.1", "****");

	signal(SIGINT, &sig_handler);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	// printf("sockfd = %i\n",sockfd);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	kq = kqueue();
	EV_SET(&event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE  , 0, 0, 0);

	// printf("before ident = %lu  \n",tevent.ident);

	ret = kevent(kq,&event, 1, NULL,0,NULL);
	while (1)
	{
		vec.clear();
		// printf("port = %i \n",cli_addr.sin_port);
		//  printf("tour de boucle infinie \n");
		//
		ret = kevent(kq, 0, 0, tevents,	20, NULL);
		// printf("ident = %lu \n",tevent.ident);
		for(int i = 0 ; i<ret; i++)
		{
			tevent = tevents[i];
			if (tevent.flags & EV_EOF)
			{
				// printf("premier if flags = %hu ident = %lu\n",tevent.flags,tevent.ident);
				int client_sockfd = static_cast<int>(tevent.ident);
				printf("Client has disconnected, sockfd = %d\n", client_sockfd);
				close(tevent.ident);
				server.remove_client_sockfd(static_cast<int>(tevent.ident));
			}
			else if (static_cast<int>(tevent.ident) == sockfd)
			{

				int client_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
				printf("New connection, sockfd = %d\n", client_sockfd);
				// printf("client_sockfd = %d\n", client_sockfd);
				server.new_client(client_sockfd, cli_addr);
				EV_SET(&event, client_sockfd, EVFILT_READ, EV_ADD  , 0, 0, 0);

				ret = kevent(kq, &event, 1, NULL,	0, NULL);
			}
			else
			{

				// printf("troisieme if ident = %lu \n",tevent.ident);
				bzero(buffer,256);

				// printf("troisieme if 1 \n");
				n = recv(tevent.ident,buffer,255,0);

				// printf("troisieme if 2 \n");
				if (n < 0)
					error("ERROR reading from socket");

				// printf("troisieme if 3 \n");
				// printf("Here is the message: %s\n",buffer);
				// std::cout << special_string(std::string("message: ") + buffer) << std::endl;
				// Message m = Message::parse(buffer);
				int client_sockfd = static_cast<int>(tevent.ident);
				Message::parse(buffer, &vec);

				// printf("taille de size %lu\n",vec.size());
				for (std::vector<Message>::iterator it = vec.begin(); it != vec.end(); it++)
				{
					// std::cout<<"message : "<<it->to_string()<<std::endl;
					// std::cout << special_string(std::string() + "message : " + it->to_string()) << std::endl;
					server.receive_message(client_sockfd, *it);
				}
			}
		}
	}
	return 0;
}
