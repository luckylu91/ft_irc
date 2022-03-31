#include <map>
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
#include "IOManager.hpp"


//
// void	IOManager::new_connection(Server * server)
// {
	// struct sockaddr_in cli_addr;
	// int temp_cli_sockfd =  accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	// printf("New connection, sockfd = %d\n", temp_cli_sockfd);
	// server->new_client(temp_cli_sockfd, cli_addr);
	// fcntl(temp_cli_sockfd, F_SETFL, O_NONBLOCK);
	// EV_SET(&triggers, temp_cli_sockfd, EVFILT_READ, EV_ADD, 0, 0, 0);
	// kevent(kq, &triggers, 1, NULL, 0, NULL);
// }
//
// void	IOManager::incoming_message(Server * server, struct kevent tevent)
// {
//
	// char buffer[256];
	// std::vector<Message> parsed_message;
	// int n;
	// int temp_cli_sockfd  = static_cast<int>(tevent.ident);
	// bzero(buffer, 256);
	// n = recv(tevent.ident, buffer, 255, 0);
	// if (n < 0)
		// error("ERROR reading from socket");
	// temp_cli_sockfd  = static_cast<int>(tevent.ident);
	// Message::parse(buffer, &parsed_message, temp_cli_sockfd   );
	// for (std::vector<Message>::iterator it = parsed_message.begin(); it != parsed_message.end(); it++)
		// server->receive_message(temp_cli_sockfd, *it);
// }
// void	IOManager::add_message_to_fd(Client const *  client, Message const &  message)
// {
	// std::map<int,std::string>::iterator it =task_tree.find(client->get_sockfd());
	// if (it == task_tree.end())
	// {
		// task_tree.insert(std::make_pair(client->get_sockfd(),message.to_string()));
		// EV_SET(&triggers, client->get_sockfd(), EVFILT_WRITE, EV_ADD | EV_ENABLE , 0, 0, 0);
		// kevent(kq, &triggers, 1, NULL, 0, NULL);
	// }
	// else
		// (it->second).append(message.to_string());
// }
//
// void	IOManager::sending_message(struct kevent tevent)
// {
	// int n;
	// std::map<int,std::string>::iterator it;
	// it = task_tree.find(tevent.ident);
	// if (it == task_tree.end())
	// {
		// std::cout<<"Absence du fd="<<tevent.ident<<" dans l'arbre\n";
		// return;
	// }
	// std::string string_to_send = it->second;
	// if (((it->second)).empty())
	// {
		// std::cout<<"chaine vide dans sending message\n";
		// return;
	// }
	// if ((size_t)tevent.data < string_to_send.size())
	// {
		// n = write(tevent.ident,((it->second).substr(0,(size_t)tevent.data)).c_str(),tevent.data);
//
		// std::cout<<"sending message = "<<((it->second).substr(0,(size_t)tevent.data)).c_str()<<"\n";
		// if  (n < 0){
			// std::cout<<"Erreur write fd = "<<tevent.ident<<std::endl;
			// return;
		// }
		// (it->second).erase(0,(size_t)tevent.data);
	// }
	// else
	// {
		// n = write(tevent.ident,(it->second).c_str(),(it->second).size());
//
		// std::cout<<"sending message = "<<it->second<<"\n";
		// if  (n < 0){
			// std::cout<<"Erreur write fd = "<<tevent.ident<<std::endl;
			// return;
		// }
		// if	((unsigned long)n < (it->second).size())
		// {
			// (it->second).erase(0,(it->second).size());
			// return;
		// }
		// task_tree.erase(tevent.ident);
		// EV_SET(&triggers,tevent.ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
		// kevent(kq, &triggers, 1, NULL, 0, NULL);
	// }
//
// }
