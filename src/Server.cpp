#include "Server.hpp"
#include "Client.hpp"
#include "Bot.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "numeric_codes.hpp"
#include <unistd.h>
#include <ctime>
#include <cstdio>
#include <filesystem>
#include <stdlib.h>

typedef std::vector<Client *>::const_iterator client_iterator;
typedef std::vector<Channel *>::const_iterator channel_iterator;

Server::Server(std::string const & name, std::string const & version, std::string const & password):
	name(name),
	version(version),
	password(password),
	clients(),
	channels(),
	creation_time_string() {
		srand(time(NULL));
		time_t timestamp = time(NULL);
		this->creation_time_string = ctime(&timestamp);
		this->creation_time_string.erase(this->creation_time_string.find_last_not_of("\n") + 1);
	}

static void delete_one_client(Client * item) {
	delete item;
}
static void delete_one_channel(Channel * item) {
	delete item;
}
Server::~Server() {
	std::for_each(this->clients.begin(), this->clients.end(), delete_one_client);
	std::for_each(this->channels.begin(), this->channels.end(), delete_one_channel);
}

void Server::new_bot(std::string const & name, std::string const & file_name) {
	Bot * bot = new Bot(*this, name);
	bot->parse_word_file(file_name);
	this->clients.push_back(static_cast<Client *>(bot));
}

void Server::new_client(int sockfd, struct sockaddr_in addr) {
	Client * client = new Client(sockfd, addr, *this);
	this->clients.push_back(client);
}

void Server::remove_client(Client * client) {
	this->rpl_quit(client);
	for_each_in_vector<RemoveClientFromChannel>(client, this->channels);
	remove_from_vector(client, this->clients);
	delete client;
}

void Server::remove_client_sockfd(int sockfd) {
	Client * client = this->find_client_by_sockfd(sockfd);
	this->remove_client(client);
}

void Server::remove_channel(Channel * channel) {
	for_each_in_vector<RemoveChannelFromClient>(channel, this->clients);
	remove_from_vector(channel, this->channels);
	delete channel;
}

Client * Server::find_client_by_sockfd(int sockfd) const {
	client_iterator it = find_in_vector<SameSockfd>(sockfd, this->clients);
	if (it == this->clients.end())
		throw NoSuchClientSockFdException(sockfd);
	return *it;
}

Client * Server::find_client_by_nick(std::string const & nick) const {
	client_iterator it = find_in_vector<SameNick>(nick, this->clients);
	if (it == this->clients.end())
		throw NoSuchClientNickException(nick);
	return *it;
}

Channel * Server::find_channel_by_name(std::string const & name) const {
	channel_iterator it = find_in_vector<SameChannelName>(name, this->channels);
	if (it == this->channels.end())
		throw NoSuchChannelNameException(name);
	return *it;
}

bool Server::try_password(std::string const & pass) const {
	return pass == this->password;
}

void Server::send_message(Client const * client, Message const & message) const {
	client->receive_message(message);
}

void Server::receive_message(int sockfd, Message & message) {
	Client * client = this->find_client_by_sockfd(sockfd);
	std::cout << "Received message '" << special_string(message.to_string()) << "'" << std::endl;
	// std::cout<<"IN receive_message\n";
	if (message.get_command() == "NICK") {
		if (message.get_param().size() == 0)
			return this->err_nonicknamegiven(client);
		client->set_nick(message.get_param()[0]);
	}
	else if (message.get_command() == "USER") {
		if (message.get_param().size() < 4)
			return this->err_needmoreparams(client, "USER");
		client->set_user(message.get_param()[0], message.get_param()[3]);
	}
	else if (message.get_command() == "PASS") {
		if (message.get_param().size() == 0)
			return this->err_needmoreparams(client, "PASS");
		client->set_password(message.get_param()[0]);
	}
	else if (message.get_command() == "JOIN") {
		if (message.get_param().size() == 0)
			return this->err_needmoreparams(client, "JOIN");
		this->join_cmd(client, message);
	}
	else if (message.get_command() == "PRIVMSG" || message.get_command() == "NOTICE") {
		if (message.get_param().size() == 0)
			return this->err_norecipient(client, message.get_command());
		if (message.get_param().size() == 1)
			return this->err_notexttosend(client);
		this->msg_cmd(message.get_command(), client, message.get_param()[0], message.get_param()[1]);
	}
	else if (message.get_command() == "PING") {
		// ...
		this->rpl_pong(client);
	}
	else if (message.get_command() == "MODE") {
		if (message.get_param().size() < 2)
			return this->err_needmoreparams(client, "MODE");
		this->mode_cmd(client, message);
	}
	else if (message.get_command() == "INVITE") {
		if (message.get_param().size() < 2)
			return this->err_needmoreparams(client, "INVITE");
		this->invite_cmd(client, message);
	}
	else if (message.get_command() == "PART") {
		if (message.get_param().size() < 1)
			return this->err_needmoreparams(client, "PART");
		this->part_cmd(client, message);
	}
	else if (message.get_command() == "KICK") {
		if (message.get_param().size() < 2)
			return this->err_needmoreparams(client, "KICK");
		this->kick_cmd(client, message);
	}
	else if (message.get_command() == "TOPIC") {
		if (message.get_param().size() ==0)
			return this->err_needmoreparams(client, "KICK");
		this->topic_cmd(client, message);
	}
	else if (message.get_command() == "LIST") {
		this->list_cmd(client, message);
	}
}

// void Server::parse_one_comma_list(std::vector<std::string> & args, std::vector<std::string> * result_vector) {
// 	std::size_t i = 1;

// 	if (args.size() == 0)
// 		throw "Badly formated";
// 	result_vector->push_back(args[0]);
// 	while (i < args.size() && (args[i] == ",")) {
// 		i++;
// 		if (args.size() == i)
// 			throw "Badly formated";
// 		result_vector->push_back(args[i]);
// 		i++;


// void Server::parse_one_comma_list(std::string const & args, std::vector<std::string> * result_vector) {
// 	std::size_t i = 1;

// 	if (args.size() == 0)
// 		throw "Badly formated";
// 	result_vector->push_back(args[0]);
// 	while (i < args.size() && (args[i] == ",")) {
// 		i++;
// 		if (args.size() == i)
// 			throw "Badly formated";
// 		result_vector->push_back(args[i]);
// 		i++;
// 	}
// 	args.erase(args.begin(), args.begin() + i);
// }

Channel * Server::try_action_on_channel_name(Client const * client, std::string const & channel_name) {
	Channel * channel;
	try {
		channel = this->find_channel_by_name(channel_name);
	}
	catch (NoSuchChannelNameException &) {
		this->err_nosuchchannel(client, channel_name);
		return NULL;
	}
	if (!channel->contains_client(client)) {
		this->err_notonchannel(client, channel);
		return NULL;
	}
	return channel;
}

bool Server::nick_exists(std::string const & nick) const {
	client_iterator it = find_in_vector<SameNick>(nick, this->clients);
	return (it != this->clients.end());
}


Message Server::base_message(Client const * client, std::string const & command) const {
	Message message;
	message.set_source(this->name);
	message.set_command(command);
	message.add_param(client->get_nick());
	return message;
}

Message Server::base_message_no_nick(std::string const & command) const {
	Message message;
	message.set_source(this->name);
	message.set_command(command);
	return message;
}



