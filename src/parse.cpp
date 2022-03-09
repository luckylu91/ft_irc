#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "parse.hpp" 
using namespace std;
void Message::parse(char *base)
{
	char *temp =  strtok(base," ");
	if (temp[0] == ':')
	{
		source = temp;
		temp =  strtok(NULL, " ");
		if (temp != NULL)
			command = base;
	}
	else
	{
		command = base;
	}
	temp =  strtok(NULL," ");
	while(temp != NULL)
	{
		if (temp[0] == ':')
		{
			param.push_back(temp);
			temp =  strtok(NULL," ");
			while(temp!= NULL)
			{
				(param.back()).append(temp);
				temp =  strtok(NULL," ");
			}
			return;
		}
		param.push_back(temp);
		temp =  strtok(NULL," ");
	}
}

Message::Message(char *base)
{
	char *temp =  strtok(base," ");
	if (temp[0] == ':')
	{
		source = temp;
		temp =  strtok(NULL, " ");
		if (temp != NULL)
			command = base;
	}
	else
	{
		command = base;
	}
	temp =  strtok(NULL," ");
	while(temp != NULL)
	{

		param.push_back(temp);
		temp =  strtok(NULL," ");
	}
}

ostream& operator<<( ostream& out, Message m)
{
	out <<"source ="<<m.source << endl
		<<"command ="<<m.command<< endl;
	out <<"param =";
	for ( vector< string>::iterator it = m.param.begin() ; it != m.param.end() ; it++)
		out << *it<<" ";
	out << endl;
	return out;
}

