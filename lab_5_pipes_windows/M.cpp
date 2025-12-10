#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    int x;
    bool first = true;
    while (iss >> x) {
      if (!first) {
        std::cout << " ";
      }
      std::cout << x * 7;
      first = false;
    }
    std::cout << std::endl;
  }
  return 0;
}