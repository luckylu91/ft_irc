#include <ctime>
#include <iostream>
#include <string>


void special_print(std::string const & s) {
	std::size_t i;

	for (i = 0; i < s.size(); i++) {
		if (!std::isspace(s[i]) || s[i] == ' ')
			std::cout << s[i];
		else {
			if (s[i] == '\f')
				std::cout << "\\f";
			else if (s[i] == '\n')
				std::cout << "\\n";
			else if (s[i] == '\r')
				std::cout << "\\r";
			else if (s[i] == '\t')
				std::cout << "\\t";
			else if (s[i] == '\v')
				std::cout << "\\v";
		}
	}
	std::cout << std::endl;
}

int main() {
	time_t timestamp = time( NULL );
	special_print(std::string(ctime(&timestamp)));
}
