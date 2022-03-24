#include "Bot.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "utils.hpp"

void Bot::receive_message(Message const & message) const {
	if (message.get_command() == "PRIVMSG" || message.get_command() == "NOTICE") {
		std::string response = select_randomly(this->phrases);
		this->server.msg_cmd("NOTICE", static_cast<Client const *>(this), message.get_source(), response);
	}
}

void Bot::parse_word_file(std::string const & file_name) {
	extract_lines(file_name, &this->phrases);
}

