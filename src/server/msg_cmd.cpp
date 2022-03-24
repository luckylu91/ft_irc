#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "errors.hpp"
#include "utils_template.tpp"

void Server::msg_cmd(std::string const & command, Client const * src, std::string const & msgtarget, std::string const & message_str) {
	try {
		Client * dest_client = this->find_client_by_nick(msgtarget);
		this->rpl_msg(command, src, dest_client, message_str);
	}
	catch (NoSuchClientNickException &) {
		try {
			Channel * dest_channel = this->find_channel_by_name(msgtarget);
			// dest_channel->forward_message(src, message_str);
			if (!dest_channel->contains_client(src))
				return this->err_cannotsendtochan(src, msgtarget);
			this->rpl_msg(command, src, dest_channel, message_str);
		}
		catch (NoSuchChannelNameException &) {
			this->err_nosuchnick(src, msgtarget);
		}
	}
}
