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
	std::cout << "Sending message '" << special_string(message.to_string()) << "'" << std::endl;
	// std::cout<<"debug dans cliend.cpp send message\n message = "<<message.to_string()<<std::endl;

	int n = write(client->get_sockfd(), message_str.c_str(), message_str.size());
	// std::cout<<"debug dans send message string = "<<message_str.c_str()<<" n ="<<n<<std::endl;
	if (n < 0)
		throw ClientSocketWriteException(client);
}

void Server::receive_message(int sockfd, Message const & message) {
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
		parse_exe_join(client, message);
	}
	else if (message.get_command() == "PRIVMSG" || message.get_command() == "NOTICE") {
		if (message.get_param().size() == 0)
			return this->err_norecipient(client, message.get_command());
		if (message.get_param().size() == 1)
			return this->err_notexttosend(client);
		this->privmsg(client, message.get_param()[0], message.get_param()[1]);
	}
	else if (message.get_command() == "PING") {
		// ...
		this->rpl_pong(client);
	}
	else if (message.get_command() == "MODE") {
		if (message.get_param().size() == 0)
			return this->err_needmoreparams(client, "MODE");
		mode_cmd(client, message);
	}
}
void	Server::mode_cmd(Client * client, Message const & message)
{
	std::vector<std::string> temp_param = message.get_param();
	Channel * channel;
	if(temp_param[0][0]=='#')
	{
		try{
			channel = find_channel_by_name(temp_param[0]);
		if(!is_in_vector(client, channel->get_operators()))
		{
			err_chanoprivsneeded(client, channel);
			return;
		}
			channel->mode_cmd_channel(client,message);
		}
		catch (NoSuchChannelNameException &)
		{
			this->err_nosuchchannel(client, temp_param[0]);
		}
		catch (NoSuchClientNickException &) 
		{
			this->err_nosuchnick(client, temp_param[2]);
		}
	}
	/*else()
	{}*/
}

void	Server::parse_exe_join(Client * client, Message const & message)
{
	std::string temp = (message.get_param())[0];
	std::string chan_name;
	size_t find_value;
	while(!temp.empty())
	{
		find_value = temp.find(',');
		if (find_value != std::string::npos)
		{
			chan_name = temp.substr(find_value);
			temp.erase(find_value+1);
		}
		else
		{
			chan_name = temp;
			temp.clear();
		}
		if (Channel::invalid_channel_name(chan_name)) {
			this->err_nosuchchannel(client, chan_name);
			continue ;
		}
		join_cmd(client, chan_name);
	}
}

void Server::join_cmd(Client * client, std::string chan_name)
{
	Channel * channel;
	bool already_in_channel = false;
	try {
		channel = this->find_channel_by_name(chan_name);
		already_in_channel = channel->contains_client(client);
		if(channel->get_is_invite_only() && is_in_vector(client, channel->get_invited_vec())) 
		{
			err_inviteonlychan(client,chan_name);
			return;
		}
		add_if_no_in(client, channel->get_clients());
		add_if_no_in(channel, client->get_channels());
	}
	catch (NoSuchChannelNameException &) {
		channel = new Channel(*this, chan_name, client);
		channels.push_back(channel);
	}
	if (!already_in_channel) {
		this->rpl_join(client, channel);
		this->rpl_notopic(client, channel);
		this->rpl_namreply(client, channel);
		this->rpl_endofnames(client, channel);
	}
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
			// std::cout<<"debug dans privnessage channel message\n";
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

Message Server::base_message(Client const * client, std::string const & command) const {
	Message message;
	message.set_source(this->name);
	message.set_command(command);
	message.add_param(client->get_nick());
	return message;
}
//
// Message Server::base_message(Channel const * channel, std::string const & command) const {
	// Message message;
	// message.set_source(this->name);
	// message.set_command(command);
	// message.add_param(client->get_name());
	// return message;
// }
Message Server::base_message_no_nick(std::string const & command) const {
	Message message;
	message.set_source(this->name);
	message.set_command(command);
	return message;
}

void Server::rpl_welcome(Client const * client) const {
	Message m = this->base_message(client, RPL_WELCOME);
	m.add_param("Welcome to the Internet Relay Network " + client->name());
	this->send_message(client, m);
}
void Server::rpl_yourhost(Client const * client) const {
	Message m = this->base_message(client, RPL_YOURHOST);
	m.add_param("Your host is " + this->name + " running version " + this->version);
	this->send_message(client, m);
}
void Server::rpl_created(Client const * client) const {
	Message m = this->base_message(client, RPL_CREATED);
	m.add_param("This server was created " + this->creation_time_string);
	this->send_message(client, m);
}
void Server::rpl_myinfo(Client const * client) const {
	Message m = this->base_message(client, RPL_MYINFO);
	m.add_param(this->name + " " + this->version); // + "<available user modes> <available channel modes>");
	this->send_message(client, m);
}

//RPL JOIN

void Server::rpl_join(Client const * client, Channel const * chan) const {
	Message m = this->base_message(client, "JOIN");
	m.add_param(chan->get_name());
	this->send_message(client, m);
}
void Server::rpl_notopic(Client const * client, Channel const * chan) const {
	Message m = this->base_message(client, RPL_NOTOPIC);
	m.add_param(chan->get_name());
	m.add_param("No topic is set");
	this->send_message(client, m);
}
void Server::rpl_namreply(Client const * client, Channel const * chan) const {
	Message m = this->base_message(client, RPL_NAMREPLY);
	m.add_param("@");
	m.add_param(chan->get_name());
	m.add_param(chan->op_cli_message());
	// std::cout<<"debug rpl_namreply ="<<m.to_string()<<std::endl;
	this->send_message(client, m);
}

void Server::rpl_endofnames(Client const * client, Channel const * chan) const {
	Message m = this->base_message(client, RPL_ENDOFNAMES);
	m.add_param(chan->get_name());
	m.add_param("End of NAMES list");
	this->send_message(client, m);
}

// RPL PONG

void Server::rpl_pong(Client const * client) const {
	Message m = this->base_message_no_nick("PONG");
	m.add_param(this->name);
	this->send_message(client, m);
}

// ERR

void Server::err_needmoreparams(Client const * client, std::string const & command) const {
	Message m = this->base_message(client, ERR_NEEDMOREPARAMS);
	m.add_param(command);
	m.add_param("Not enough parameters");
	this->send_message(client, m);
}
void Server::err_alreadyregistred(Client const * client) const {
	Message m = this->base_message(client, ERR_ALREADYREGISTRED);
	m.add_param("Unauthorized command (already registered)");
	this->send_message(client, m);
}
void Server::err_nonicknamegiven(Client const * client) const {
	Message m = this->base_message(client, ERR_NONICKNAMEGIVEN);
	m.add_param("No nickname given");
	this->send_message(client, m);
}
void Server::err_erroneusnickname(Client const * client, std::string const & nick) const {
	Message m = this->base_message(client, ERR_ERRONEUSNICKNAME);
	m.add_param(nick);
	m.add_param("Erroneous nickname");
	this->send_message(client, m);
}
void Server::err_nicknameinuse(Client const * client, std::string const & nick) const {
	Message m = this->base_message(client, ERR_NICKNAMEINUSE);
	m.add_param(nick);
	m.add_param("Nickname is already in use");
	this->send_message(client, m);
}
void Server::err_restricted(Client const * client) const {
	Message m = this->base_message(client, ERR_RESTRICTED);
	m.add_param("Your connection is restricted!");
	this->send_message(client, m);
}
void Server::err_passwdmismatch(Client const * client) const {
	Message m = this->base_message(client, ERR_PASSWDMISMATCH);
	m.add_param("Password incorrect");
	this->send_message(client, m);
}
void Server::err_nosuchnick(Client const * client, std::string const & nick) const {
	Message m = this->base_message(client, ERR_NOSUCHNICK);
	m.add_param(nick);
	m.add_param("No such nick/channel");
	this->send_message(client, m);
}

void Server::err_norecipient(Client const * client, std::string const & command) const {
	Message m = this->base_message(client, ERR_NORECIPIENT);
	m.add_param("No recipient given (" + command + ")");
	this->send_message(client, m);
}

void Server::err_chanoprivsneeded(Client const * client,Channel const * channel) const {
	Message m = this->base_message(client, ERR_CHANOPRIVSNEEDED);
	m.add_param(channel->get_name());
	m.add_param("You're not channel operator");
	this->send_message(client, m);
}

void Server::err_notexttosend(Client const * client) const {
	Message m = this->base_message(client, ERR_NOTEXTTOSEND);
	m.add_param("No text to send");
	this->send_message(client, m);
}

void Server::err_nosuchchannel(Client const * client, std::string const & channel_name) const {
	Message m = this->base_message(client, ERR_NOSUCHCHANNEL);
	m.add_param(channel_name);
	m.add_param("No such channel");
	this->send_message(client, m);
}

void Server::err_inviteonlychan(Client const * client, std::string const & channel_name) const {
	Message m = this->base_message(client, ERR_INVITEONLYCHAN);
	m.add_param(channel_name);
	m.add_param("Cannot join channel (+i)");
	this->send_message(client, m);
}
void Server::err_unknownmode(Client const * client, std::string const & flag,std::string const & channel_name) const {
	Message m = this->base_message(client,ERR_UNKNOWNMODE);
	m.add_param(flag);
	m.add_param("is unknown mode char to me for");
	m.add_param(channel_name);
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
