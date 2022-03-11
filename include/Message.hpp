#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <iostream>

class Message
{
public:
  Message();
  void set_source(std::string const & source);
  void set_command(std::string const & command);
  std::string get_source() const { return this->source; }
  std::string get_command() const { return this->command; }
  std::vector<std::string> const & get_param() const { return this->param; }
  void add_param(std::string const & param);

  static void parse(char *base, std::vector<Message> vec);

  static Message parse_one(std::string s);

  std::string to_string() const;
	friend std::ostream& operator<<(std::ostream & out, Message const & m);

private:
  bool source_is_set;
	std::string source;
	std::string command;
	std::vector<std::string> param;
};

