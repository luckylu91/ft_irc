#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include "Message.hpp"

typedef std::vector<std::string>::const_iterator string_iterator;

Message::Message(): source_is_set(false) {}


void Message::set_source(std::string const & source) {
  this->source = source;
  this->source_is_set = true;
}
void Message::set_command(std::string const & command) {
  this->command = command;
}
void Message::add_param(std::string const & param) {
  this->param.push_back(param);
}

void Message::parse(char *base, std::vector<Message> vec)
{
	std::string temp = base;
	int start =0;
	int end;
	while ((end = temp.find("\r\n",start))!=std::string::npos)
	{
		vec.push_back(parse_one(temp.substr(start,end)));
		start = end+2;
	}
	

}


Message Message::parse_one(std::string s)
{
  Message message;
	char * base = &s[0];
  char *temp =  strtok(base," ");
	if (temp[0] == ':')
	{
		message.set_source(temp);
		temp =  strtok(NULL, " ");
		if (temp != NULL)
			message.set_command(base);
	}
	else
	{
		message.set_command(base);
	}
	temp =  strtok(NULL," ");
	while(temp != NULL)
	{
		if (temp[0] == ':')
		{
			message.param.push_back(temp);
			temp =  strtok(NULL," ");
			while(temp!= NULL)
			{
				(message.param.back()).append(temp);
				temp =  strtok(NULL," ");
			}
			return message;
		}
		message.param.push_back(temp);
		temp =  strtok(NULL," ");
	}
  return message;
}

static bool param_has_spaces(std::string const & str) {
  return str.find(' ') != std::string::npos;
}

static void insert_params(std::stringstream & ss, std::vector<std::string> const & param) {
  if (param.size() == 0)
    return;
  string_iterator it_last = --param.end();
  for (string_iterator it = param.begin(); it != it_last; ++it) {
    ss << " " << *it;
  }
  ss << " ";
  if (param_has_spaces(*it_last))
    ss << ":";
  ss << *it_last;
}

std::string Message::to_string() const {
  std::stringstream ss;

  if (this->source_is_set)
    ss << ":" << this->source << " ";
  ss << this->command;
  insert_params(ss, this->param);
  return ss.str();
}

// ostream& operator<<( ostream& out, Message m)
// {
// 	out <<"source ="<<m.source << endl
// 		<<"command ="<<m.command<< endl;
// 	out <<"param =";
// 	for ( vector< string>::iterator it = m.param.begin() ; it != m.param.end() ; it++)
// 		out << *it<<" ";
// 	out << endl;
// 	return out;
// }

