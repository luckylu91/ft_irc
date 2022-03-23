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

bool Server::try_password(std::string const & pass) const {
	return pass == this->password;
}

void Server::send_message(Client const * client, Message const & message) const {
	std::string message_str = message.to_string();
	std::cout << "Sending message  '" << special_string(message.to_string()) << "'" << std::endl;
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
		this->msg_cmd(client, message.get_param()[0], message.get_param()[1]);
	}
	else if (message.get_command() == "PING") {
		// ...
		this->rpl_pong(client);
	}
	else if (message.get_command() == "KICK") {
		if (message.get_param().size() < 2)
			return this->err_needmoreparams(client, "KICK");
		this->kick_cmd(client, message);
	}
	else if (message.get_command() == "PART") {
		if (message.get_param().size() < 1)
			return this->err_needmoreparams(client, "PART");
		this->part_cmd(client, message);
	}
}

void	Server::parse_exe_join(Client * client, Message const & message)
{
	std::string temp = (message.get_param())[0];
	std::string chan_name;
	size_t f;
	while(!temp.empty())
	{
		f = temp.find(',');
		if (f != std::string::npos)
		{
			chan_name = temp.substr(f);
			temp.erase(f+1);
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
void Server::msg_cmd(Client const * src, std::string const & msgtarget, std::string const & message) {
	try {
		Client * dest_client = this->find_client_by_nick(msgtarget);
		src->send_message(dest_client, message);
	}
	catch (NoSuchClientNickException &) {
		try {
			Channel * dest_channel = this->find_channel_by_name(msgtarget);
			// std::cout<<"debug dans privnessage channel message\n";
			this->msg_channel(src, dest_channel, message);
		}
		catch (NoSuchChannelNameException &) {
			this->err_nosuchnick(src, msgtarget);
		}
	}
}
void Server::msg_channel(Client const * src, Channel const * dest, std::string const & message) const {
	dest->forward_message(src, message);
}

static void parse_one_comma_list(std::vector<std::string> & args, std::vector<std::string> * result_vector) {
	std::size_t i = 1;

	if (args.size() == 0)
		throw "Badly formated";
	result_vector->push_back(args[0]);
	while (i < args.size() && (args[i] == ",")) {
		i++;
		if (args.size() == i)
			throw "Badly formated";
		result_vector->push_back(args[i]);
		i++;
	}
	args.erase(args.begin(), args.begin() + i);
}

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

void Server::part_cmd(Client * client, Message const & message) {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channels, dests;

	try {
		parse_one_comma_list(args, &channels);
		std::string part_message = args.size() > 0 ? args[0] : client->get_nick();
		for (std::size_t i = 0; i < channels.size(); i++) {
			this->part_one_cmd(client, channels[i], part_message);
		}
	}
	catch (...) {
		return ;
	}
}

void Server::part_one_cmd(Client * client, std::string const & channel_name, std::string const & part_message) {
	Channel * channel = try_action_on_channel_name(client, channel_name);
	if (channel == NULL)
		return ;
	channel->remove_client(client);
	this->msg_channel(client, channel, part_message);
}


void Server::kick_cmd(Client * src, Message const & message) {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channels, dests;

	try {
		parse_one_comma_list(args, &channels);
		parse_one_comma_list(args, &dests);
		if (channels.size() > 1 && channels.size() != dests.size())
			return ;
		std::string * part_message = args.size() > 0 ? &args[0] : NULL;
		for (std::size_t i = 0; i < dests.size(); i++) {
			std::string channel = channels.size() == 1 ? channels[0] : channels[i];
			std::string dest = dests[i];
			this->kick_one_cmd(src, channel, dest, part_message);
		}
	}
	catch (...) {
		return ;
	}
}

void Server::kick_one_cmd(Client * src, std::string const & channel_name, std::string const & dest_name, std::string const * part_message_option) {
	Channel * channel = try_action_on_channel_name(src, channel_name);
	if (channel == NULL)
		return ;
	if (!channel->is_operator(src))
		return this->err_chanoprivsneeded(src, channel);
	try {
		Client * dest = this->find_client_by_nick(dest_name);
		if (!channel->contains_client(dest))
			throw NoSuchClientException();
		std::string part_message = part_message_option != NULL ? *part_message_option : dest->get_nick();
		this->part_one_cmd(dest, channel_name, part_message);
	}
	catch (NoSuchClientException &) {
		return this->err_usernotinchannel(src, dest_name, channel);
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
	Message m;
	m.set_source(client->name());
	m.set_command("JOIN");
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

void Server::err_chanoprivsneeded(Client const * client, Channel const * channel) const {
	Message m = this->base_message(client, ERR_CHANOPRIVSNEEDED);
	m.add_param(channel->get_name());
	m.add_param("You're not channel operator");
	this->send_message(client, m);
}
void Server::err_usernotinchannel(Client const * client, std::string const & nick, Channel const * channel) const {
	Message m = this->base_message(client, ERR_USERNOTINCHANNEL);
	m.add_param(nick);
	m.add_param(channel->get_name());
	m.add_param("They aren't on that channel");
	this->send_message(client, m);
}
void Server::err_notonchannel(Client const * client, Channel const * channel) const {
	Message m = this->base_message(client, ERR_NOTONCHANNEL);
	m.add_param(channel->get_name());
	m.add_param("You're not on that channel");
	this->send_message(client, m);
}
void Server::err_badchanmask(Client const * client, Channel const * channel) const {
	Message m = this->base_message(client, ERR_BADCHANMASK);
	m.add_param(channel->get_name());
	m.add_param("Bad Channel Mask");
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
}

void RemoveChannelFromClient::operator()(Channel * channel, Client * client) {
	client->remove_channel(channel);
}
