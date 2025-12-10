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
      std::cout << x * x * x;
      first = false;
    }
    std::cout << std::endl;
  }
  return 0;
}