#include <string>
#include "_MockIOManager.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include "utils.hpp"


int main() {
  std::ofstream output_file("out.txt");
  MockIOManager io_manager(output_file);
  Server server("LE_SERVER", "0.1", "root", &io_manager);
  //...
}
