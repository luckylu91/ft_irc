#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "utils.hpp"
#include "errors.hpp"
#include "Channel.hpp"
#include "numeric_codes.hpp"
#include <unistd.h>
#include <ctime>
#include <cstdio>

typedef std::vector<Client *>::const_iterator client_iterator;
typedef std::vector<Channel *>::const_iterator channel_iterator;

Server::Server(std::string const & name, std::string const & version, std::string const & password):
	name(name),
	version(version),
	password(password),
	clients() {
		time_t timestamp = time(NULL);
		this->creation_time_string = ctime(&timestamp);
		this->creation_time_string.erase(this->creation_time_string.find_last_not_of("\n") + 1);
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

void Server::new_client(int sockfd, struct sockaddr_in addr) {
	Client * client = new Client(sockfd, addr, *this);
	this->clients.push_back(client);
}

void Server::remove_client(Client * client) {
	for_each_in_vector<RemoveClientFromChannel>(client, this->channels);
	remove_from_vector(client, this->clients);
}

void Server::remove_client_sockfd(int sockfd) {
	Client * client = this->find_client_by_sockfd(sockfd);
	this->remove_client(client);
}

bool Server::try_password(std::string const & password) const {
	return password == this->password;
}

void Server::send_message(Client const * client, Message const & message) const {
	std::string message_str = message.to_string();
	std::cout << "Sending message '" << message.to_string_striped() << "'" << std::endl;
	// std::cout<<"debug dans cliend.cpp send message\n message = "<<message.to_string()<<std::endl;

	int n = write(client->get_sockfd(), message_str.c_str(), message_str.size());
	// std::cout<<"debug dans send message string = "<<message_str.c_str()<<" n ="<<n<<std::endl;
	if (n < 0)
		throw ClientSocketWriteException(client);
}

void Server::receive_message(int sockfd, Message const & message) {
	Client * client = this->find_client_by_sockfd(sockfd);
	std::cout << "Received message '" << message.to_string_striped() << "'" << std::endl;
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

//	std::cout<<"IN JOIN 2\n";
		std::string temp = (message.get_param())[0];
		std::string chan_name;
		size_t f;
		while(!temp.empty())
		{
//	std::cout<<"IN JOIN 3\n";
			f = temp.find(',');

//	std::cout<<"IN JOIN 4\n";

//	std::cout<<"IN JOIN 3.4\n";
			if (f != std::string::npos)
			{
//	std::cout<<"IN JOIN 3.3\n";

				chan_name = temp.substr(f);
				temp.erase(f+1);
			}
			else
			{
//	std::cout<<"IN JOIN 4 "<<temp<<"\n";
				chan_name = temp;
				temp.clear();

//	std::cout<<"IN JOIN 4.4\n";
			}
			if (Channel::invalid_channel_name(chan_name)) {
				this->err_nosuchchannel(client, chan_name);
				continue ;
			}
			join_cmd(client, chan_name);
			rpl_join(client,find_channel_by_name(chan_name));
			rpl_notopic(client,find_channel_by_name(chan_name));
			rpl_namreply(client,find_channel_by_name(chan_name));
// 			if(	join_cmd(client,chan_name))
// 			{

// //	std::cout<<"IN JOIN 5\n";
// 				rpl_join(client,find_channel_by_name(chan_name));
// 				rpl_notopic(client,find_channel_by_name(chan_name));
// 				rpl_namreply(client,find_channel_by_name(chan_name));
// 			}
// 			else
// 			{
// 				//bug creating chan
// 				std::cout<<"Bug creating chan\n";
// 			}

//	std::cout<<"IN JOIN 6\n";
		}

	}
	else if (message.get_command() == "PRIVMSG") {
		if (message.get_param().size() == 0)
			return this->err_norecipient(client, "PRIVMSG");
		if (message.get_param().size() == 1)
			return this->err_notexttosend(client);
		this->privmsg(client, message.get_param()[0], message.get_param()[1]);
	}
	else if (message.get_command() == "PING") {
		// ...
		this->rpl_pong(client);
	}
}

void Server::join_cmd(Client * client, std::string chan_name)
{
	try {
		Channel * channel = this->find_channel_by_name(chan_name);
		add_if_no_in(client, channel->get_clients());
		add_if_no_in(channel, client->get_channels());
	}
	catch (NoSuchClientException &) {
		channels.push_back(new Channel(*this, chan_name, client));
	}
	// for(std::vector<Channel *>::iterator it = channels.begin(); it != channels.end();it++)
	// {
	// 	if ((*it)->get_name() == chan_name)
	// 	{
	// 		add_if_no_in(client, (*it)->get_clients());
	// 		add_if_no_in(*it, client->get_channels());
	// 		return 1;
	// 	}
	// }
	// channels.push_back(new Channel(*this, chan_name,client));
	// return 1;
}

// ERR_NORECIPIENT
// ERR_NOTEXTTOSEND
// ERR_CANNOTSENDTOCHAN
// ERR_NOTOPLEVEL
// ERR_WILDTOPLEVEL
// ERR_TOOMANYTARGETS
// ERR_NOSUCHNICK
// RPL_AWAY
void Server::privmsg(Client const * src, std::string const & msgtarget, std::string const & message) {
	try {
		Client * dest_client = this->find_client_by_nick(msgtarget);
		src->send_message(dest_client, message);
	}
	catch (NoSuchClientNickException &) {
		try {
			Channel * dest_channel = this->find_channel_by_name(msgtarget);
			std::cout<<"debug dans privnessage channel message\n";
			dest_channel->forward_message(src, message);// this->msg_channel(src, dest_channel, message);
		}
		catch (NoSuchChannelNameException &) {
			this->err_nosuchnick(src, msgtarget);
		}
	}
}

void Server::welcome(Client const * client) const {
	this->rpl_welcome(client);
	this->rpl_yourhost(client);
	this->rpl_created(client);
	this->rpl_myinfo(client);
}

bool Server::nick_exists(std::string const & nick) const {
	client_iterator it = find_in_vector<SameNick>(nick, this->clients);
	return (it != this->clients.end());
}

Message Server::base_message(std::string const & command) const {
	Message message;
	message.set_source(this->name);
	message.set_command(command);
	return message;
}

void Server::rpl_welcome(Client const * client) const {
	Message m = this->base_message(RPL_WELCOME);

	m.add_param("Welcome to the Internet Relay Network " + client->name());
	this->send_message(client, m);
}
void Server::rpl_yourhost(Client const * client) const {
	Message m = this->base_message(RPL_YOURHOST);
	m.add_param("Your host is " + this->name + " running version " + this->version);
	this->send_message(client, m);
}
void Server::rpl_created(Client const * client) const {
	Message m = this->base_message(RPL_CREATED);
	m.add_param("This server was created " + this->creation_time_string);
	this->send_message(client, m);
}
void Server::rpl_myinfo(Client const * client) const {
	Message m = this->base_message(RPL_MYINFO);
	m.add_param(this->name + " " + this->version); // + "<available user modes> <available channel modes>");
	this->send_message(client, m);
}

//RPL JOIN

void Server::rpl_join(Client const * client, Channel const * chan) const {
	Message m = this->base_message("JOIN");
	m.add_param(chan->get_name());
	this->send_message(client, m);
}
void Server::rpl_notopic(Client const * client, Channel const * chan) const {
	Message m = this->base_message(RPL_NOTOPIC);
	m.add_param(chan->get_name());
	m.add_param(":No topic is set\r\n");
	this->send_message(client, m);
}
void Server::rpl_namreply(Client const * client, Channel const * chan) const {
	Message m = this->base_message(RPL_NAMREPLY);
	m.add_param(client->get_nick());
	m.add_param("=");
	m.add_param(chan->get_name());
	m.add_param(chan->op_cli_message());
	std::cout<<"debug rpl_namreply ="<<m.to_string()<<std::endl;
	this->send_message(client, m);
}

// RPL PONG

void Server::rpl_pong(Client const * client) const {
	Message m = this->base_message("PONG");
	m.add_param(this->name);
	m.add_param(client->name());
	this->send_message(client, m);
}

// ERR

void Server::err_needmoreparams(Client const * client, std::string const & command) const {
	Message m = this->base_message(ERR_NEEDMOREPARAMS);
	m.add_param(client->name());
	m.add_param(command);
	m.add_param("Not enough parameters");
	this->send_message(client, m);
}
void Server::err_alreadyregistred(Client const * client) const {
	Message m = this->base_message(ERR_ALREADYREGISTRED);
	m.add_param(client->name());
	m.add_param("Unauthorized command (already registered)");
	this->send_message(client, m);
}
void Server::err_nonicknamegiven(Client const * client) const {
	Message m = this->base_message(ERR_NONICKNAMEGIVEN);
	m.add_param(client->name());
	m.add_param("No nickname given");
	this->send_message(client, m);
}
void Server::err_erroneusnickname(Client const * client, std::string const & nick) const {
	Message m = this->base_message(ERR_ERRONEUSNICKNAME);
	m.add_param(client->name());
	m.add_param(nick);
	m.add_param("Erroneous nickname");
	this->send_message(client, m);
}
void Server::err_nicknameinuse(Client const * client, std::string const & nick) const {
	Message m = this->base_message(ERR_NICKNAMEINUSE);
	m.add_param(client->name());
	m.add_param(nick);
	m.add_param("Nickname is already in use");
	this->send_message(client, m);
}
void Server::err_restricted(Client const * client) const {
	Message m = this->base_message(ERR_RESTRICTED);
	m.add_param(client->name());
	m.add_param("Your connection is restricted!");
	this->send_message(client, m);
}
void Server::err_passwdmismatch(Client const * client) const {
	Message m = this->base_message(ERR_PASSWDMISMATCH);
	m.add_param(client->name());
	m.add_param("Password incorrect");
	this->send_message(client, m);
}
void Server::err_nosuchnick(Client const * client, std::string const & nick) const {
	Message m = this->base_message(ERR_NOSUCHNICK);
	m.add_param(client->name());
	m.add_param(nick);
	m.add_param("No such nick/channel");
	this->send_message(client, m);
}

void Server::err_norecipient(Client const * client, std::string const & command) const {
	Message m = this->base_message(ERR_NORECIPIENT);
	m.add_param(client->name());
	m.add_param("No recipient given (" + command + ")");
	this->send_message(client, m);
}

void Server::err_notexttosend(Client const * client) const {
	Message m = this->base_message(ERR_NOTEXTTOSEND);
	m.add_param(client->name());
	m.add_param("No text to send");
	this->send_message(client, m);
}

void Server::err_nosuchchannel(Client const * client, std::string const & channel_name) const {
	Message m = this->base_message(ERR_NOSUCHCHANNEL);
	// m.add_param(client->name());
	m.add_param(channel_name);
	m.add_param("No such channel");
	this->send_message(client, m);
}


bool SameNick::operator()(std::string const & nick, Client const * client) {
	return nick == client->get_nick();
}

bool SameSockfd::operator()(int sockfd, Client const * client) {
	return sockfd == client->get_sockfd();
}

bool SameChannelName::operator()(std::string const & name, Channel const * channel) {
	return name == channel->get_name();
}

void RemoveClientFromChannel::operator()(Client * client, Channel * channel) {
	channel->remove_client(client);
	client->remove_channel(channel);
}

