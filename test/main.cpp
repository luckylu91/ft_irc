#include <string>
#include "_MockIOManager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "utils.hpp"

bool is_connect_line(std::string const & line, int & sockfd) {
  if (line.substr(0, strlen("connect ")) != "connect ")
    return false;
  
}

int main() {
  std::ifstream input_file("test/normal_registration.txt");
  std::ofstream output_file("out.txt");
  MockIOManager io_manager(output_file);
  Server server("LE_SERVER", "0.1", "root", &io_manager);

  std::string line;
  while (std::getline(input_file, line)) {
    if (strncmp(line.c_str(), "connect ", ))
    std::cout << line << std::endl;
  }

  //...
}
