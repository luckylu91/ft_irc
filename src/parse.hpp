#include <string>
#include <vector>
#include <cstring>
#include <iostream>

struct Message
	{
		std::string source;
		std::string command;
		std::vector<std::string> param;
		Message(char *base);
		Message()
		{}
		void parse(char *base);
		std::ostream& operator<<(std::ostream& out);

		friend std::ostream& operator<<(std::ostream& out, Message m);

	};

