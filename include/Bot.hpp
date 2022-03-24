#pragma once

#include <string>
#include <cctype>
#include <vector>
#include "Client.hpp"

class Bot : public Client {
public:
	Bot(Server & server, std::string const & name): Client(server, name) {}
	virtual ~Bot() {}
	virtual void receive_message(Message const & message) const;
	void parse_word_file(std::string const & file_name);

private:
	std::vector<std::string> phrases;
};
