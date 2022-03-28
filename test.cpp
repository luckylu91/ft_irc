#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

void extract_lines(std::string const & file_name, std::vector<std::string> * lines) {
	std::fstream fs;
	std::string line;

	fs.open(file_name, std::fstream::in);
	if (fs.fail()) {
		lines->push_back("ERR_WORDS_FILE");
		return ;
	}
	while (std::getline(fs, line)) {
		if (line.empty())
			continue ;
		lines->push_back(line);
	}
	fs.close();
}
