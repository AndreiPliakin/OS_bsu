#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string line;
  long long sum = 0;

  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    int x;
    while (iss >> x) {
      sum += x;
    }
  }
  std::cout << sum << std::endl;
  return 0;
}