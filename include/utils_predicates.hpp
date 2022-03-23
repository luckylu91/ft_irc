#pragma once

#include <string>

class Client;
class Channel;
class Message;

struct SameNick {
	bool operator()(std::string const & nick, Client const * client);
};
struct SameSockfd {
	bool operator()(int sockfd, Client const * client);
};
struct SameChannelName {
	bool operator()(std::string const & name, Channel const * channel);
};
struct RemoveClientFromChannel {
	void operator()(Client * client, Channel * channel);
};
struct RemoveChannelFromClient {
	void operator()(Channel * channel, Client * client);
};

struct SendMessageToClient {
	void operator()(Message const & message, Client const * client);
};

struct SendMessageToClientDifferent {
	Client const * sender;
	Message const & message;
	SendMessageToClientDifferent(Client const * sender,	Message const & message): sender(sender), message(message) {}
	void operator()(Client const * client);
};

