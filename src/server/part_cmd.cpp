#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void Server::part_cmd(Client * client, Message const & message) {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channels, dests;

	try {
		Server::parse_one_comma_list(args, &channels);
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
	this->rpl_part_and_remove(client, channel, part_message);
}
