// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>

// lambda
int main(int argc, char* argv[])
{
  int z = 100;

  auto closure1 = [z](int a, int b) -> int { return a + b + z; };
  std::cout << closure1(5, 6) << std::endl; // 111

  //auto closure2 = [z](int a, int b) -> int { z = 0; return a + b + z; }; // error: assignment of read-only variable ‘z’
  auto closure2 = [z](int a, int b) mutable -> int { z = 0; return a + b + z; };
  std::cout << closure2(5, 6) << " z=" << z << std::endl; // 11 z=100

  auto closure3 = [z](int a, int b) -> decltype(a+b) { return a + b + z; };
  std::cout << closure3(5, 6) << std::endl; // 111

  auto closure4 = [&z](int a, int b) -> int { z = 0; return a + b + z; };
  std::cout << closure4(5, 6) << " z=" << z << std::endl; // 11 0

  return EXIT_SUCCESS;
}
