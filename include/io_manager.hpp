#pragma once
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
class Io_manager
{
	
	private:
		std::map<int,std::string> task_tree;
		int server_fd;
		struct kevent triggers;
		int kq;
		int clilen;
	public:
	Io_manager(int _server_fd, int _clilen):server_fd(_server_fd), clilen(_clilen)
	{
		kq = kqueue();
  		EV_SET(&triggers, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
   		kevent(kq, &triggers, 1, NULL, 0, NULL);
	}
	void	new_connection(Server * server);
	void	incoming_message(Server * server, struct kevent tevent);
	void	sending_message(struct kevent tevent);
	void	add_message_to_fd(Client const *  client, Message const &  message);
	int		get_kq() const {return kq;}
};
