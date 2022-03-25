#include "Bot.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "SuffixTree.hpp"
#include "utils.hpp"

void Bot::receive_message(Message const & message) const {
	if (message.get_command() != "PRIVMSG" && message.get_command() != "NOTICE") {
		// throw UnexpectedBotMessage();
	}
	else {
		std::string word = this->tri.select_random_word_with_prefix(message.get_param()[1]);
		std::string response = "Here is a word of wisdom : " + word;
		// std::string response = select_randomly(this->phrases);
		// std::cout << special_string(response) << std::endl;
		// std::cout << message.get_source_address() << std::endl;
		this->server.msg_cmd("NOTICE", static_cast<Client const *>(this), message.get_source_address()->get_nick(), response);
	}
}

void Bot::parse_word_file(std::string const & file_name) {
	std::vector<std::string> lines;

	std::cout << "(Bot) Reading words file... ";
	extract_lines(file_name, &lines);
	for (std::size_t i = 0; i < lines.size(); i++)
		tri.add_string(lines[i]);
	std::cout << "done !" << std::endl;
}

