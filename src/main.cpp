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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.hpp"
#include "Server.hpp"


void error(const char *msg)
{
	perror(msg);
	exit(0);
}
int main(int argc, char *argv[])
{
	struct	kevent event;	 /* Event we want to monitor */
	struct	kevent tevents[20];

	struct	kevent tevent;
	std::vector<Message> vec;
	int sockfd, newsockfd, portno, clilen, n, kq, ret;

	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	Server server("LE_SERVER", "0.1", "root");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd = %i\n",sockfd);
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

	printf("before ident = %lu  \n",tevent.ident);

	ret = kevent(kq,&event, 1, NULL,0,NULL);
	while (1)
	{
		printf("port = %i \n",cli_addr.sin_port);
		ret = kevent(kq, 0, 0, tevents,	20, NULL);
		printf("ident = %lu \n",tevent.ident);
		for(int i = 0 ; i<ret; i++)
		{
			tevent = tevents[i];
			if (tevent.flags & EV_EOF)
			{

				// printf("premier if flags = %hu ident = %lu\n",tevent.flags,tevent.ident);
				printf("Client has disconnected\n");
				close(tevent.ident);

			}
			else if (tevent.ident == sockfd)
			{

				printf("New connection\n");
				newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
				server.new_client(newsockfd, cli_addr);
				EV_SET(&event, newsockfd, EVFILT_READ, EV_ADD  , 0, 0, 0);

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
				printf("Here is the message: %s\n",buffer);
				// Message m = Message::parse(buffer);
				int cli_sockfd = static_cast<int>(tevent.ident);
				Message::parse(buffer,vec);
				for (std::vector<Message>::iterator it = vec.begin(); it != vec.end(); it++)
				{
					std::cout<<"message : "<<it->to_string()<<std::endl;
					server.receive_message(cli_sockfd, *it);
				}
				// std::cout<<m;
				// n = write(tevent.ident,"I got your message",18);
				// if (n < 0)
				// 	error("ERROR writing to socket");
			}
		}
	}
	return 0;
}
