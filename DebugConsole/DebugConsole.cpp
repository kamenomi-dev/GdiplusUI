
#include <Windows.h>

#include <iostream>

int main() {
  const char* text[16] = {"0", "1", "2", "3", "4", "5", "6", "7",
                          "8", "9", "A", "B", "C", "D", "E", "F"};

  const int rawCount = 4289379276;
  unsigned int result = rawCount;

  while (result != NULL) {
    auto c = result & 0b11111111;
    std::cout << c << std::endl;
    result >>= 8;
  }
}
