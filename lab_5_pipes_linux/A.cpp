#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string line;
  const int N = 42;

  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    int x;
    bool first = true;
    while (iss >> x) {
      if (!first) {
        std::cout << " ";
      }
      std::cout << x + N;
      first = false;
    }
    std::cout << std::endl;
  }
  return 0;
}