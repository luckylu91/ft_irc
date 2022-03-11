#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <iostream>

struct Message
{
  Message();
	Message(std::string source,	std::string command, std::vector<std::string> param);
  static Message parse(char *base);

  std::string to_string() const;
	friend std::ostream& operator<<(std::ostream & out, Message const & m);

  bool source_is_present;
	std::string source;
	std::string command;
	std::vector<std::string> param;
};
