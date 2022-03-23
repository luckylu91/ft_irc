#pragma once

#include <string>
#include <vector>

class Server;
class Client;
class Message;

class Channel {
public:
	Channel(Server & server, std::string const & name, Client const * oper):
		server(server),
		opers(1, oper),
		clients(),
		name(name) {
			(void)this->server;
		}

	static bool invalid_channel_name(std::string const & name);
	void remove_client(Client * client);
	std::string get_name() const { return this->name; }
	std::vector<Client const *> & get_clients() { return this->clients; }
	std::string op_cli_message() const;
	void forward_message(Client const * src, std::string const & content) const;
	bool contains_client(Client const * client) const;
	bool is_operator(Client const * client) const;
	bool is_normal_user(Client const * client) const;
	std::size_t size() const;

private:
	Server & server;
	std::vector<Client const *> opers;
	std::vector<Client const *> clients;
	std::string name;
};

struct SendMessageToClient {
	void operator()(Message const & message, Client const * client);
};
