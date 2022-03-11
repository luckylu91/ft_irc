#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <iostream>

class Message
{
  Message();
  void set_source(std::string const & source);
  void set_command(std::string const & command);
  void add_param(std::string const & param);

  static Message parse(char *base);

  std::string to_string() const;
	friend std::ostream& operator<<(std::ostream & out, Message const & m);

  bool source_is_set;
	std::string source;
	std::string command;
	std::vector<std::string> param;
};


Message err_passwdmismatch();
