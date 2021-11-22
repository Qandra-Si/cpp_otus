#include <iostream>
#include <cstdlib>

#include <core/utils.h>

int main(int argc, char* argv[])
{
  std::cout
    << "Hello World!" << std::endl
    << "We had core library built at " << core::get_build_ticker() << std::endl;
  return EXIT_SUCCESS;
}
