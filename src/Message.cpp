#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include "Message.hpp"

typedef std::vector<std::string>::const_iterator string_iterator;

Message::Message() {}

Message::Message(std::string source,	std::string command, std::vector<std::string> param):
  source(source),
  command(command),
  param(param) {}

Message Message::parse(char *base)
{
  Message message;

	char *temp =  strtok(base," ");
	if (temp[0] == ':')
	{
		message.source = temp;
		temp =  strtok(NULL, " ");
		if (temp != NULL)
			message.command = base;
	}
	else
	{
		message.command = base;
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
			return;
		}
		message.param.push_back(temp);
		temp =  strtok(NULL," ");
	}
  return message;
}

static param_has_spaces(std::string const & str) {
  str.find(' ')
}

std::string Message::to_string() const {
  std::stringstream ss;


  if (this->source_is_present)
    ss << ":" << this->source << " ";
  ss << this->command;
  for (string_iterator it = this->param.begin(); it != this->param.end(); ++it) {
    ss << " " << *it;
  }
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

