#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils_template.tpp"


void Server::kick_cmd(Client * src, Message const & message) {
	std::vector<std::string> args = message.get_param();
	std::vector<std::string> channels, dests;

	try {
		Server::parse_one_comma_list(args, &channels);
		Server::parse_one_comma_list(args, &dests);
		if (channels.size() > 1 && channels.size() != dests.size())
			return ;
		for (std::size_t i = 0; i < dests.size(); i++) {
			std::string channel = channels.size() == 1 ? channels[0] : channels[i];
			std::string dest = dests[i];
			this->kick_one_cmd(src, channel, dest, args.size() > 0 ? args[0] : dest);
		}
	}
	catch (...) {
		return ;
	}
}

void Server::kick_one_cmd(Client * src, std::string const & channel_name,
		std::string const & dest_name, std::string const & kick_message) {
	Channel * channel = try_action_on_channel_name(src, channel_name);
	if (channel == NULL)
		return ;
	if (!channel->is_operator(src))
		return this->err_chanoprivsneeded(src, channel);
	try {
		Client * dest = this->find_client_by_nick(dest_name);
		if (!channel->contains_client(dest))
			return this->err_usernotinchannel(src, dest_name, channel);
		this->rpl_kick_and_remove(src, dest, channel, kick_message);
		// this->part_one_cmd(dest, channel_name, part_message);
	}
	catch (NoSuchClientException &) {
		return this->err_usernotinchannel(src, dest_name, channel);
	}
}
