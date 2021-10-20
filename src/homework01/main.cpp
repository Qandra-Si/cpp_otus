#include <iostream>
#include <cstdlib>
#include <cpp_otus_version.h>

int main(int argc, char* argv[])
{
  std::cout
    << "build " << PROJECT_VERSION << std::endl
    << "Hello, World!" << std::endl;
  return EXIT_SUCCESS;
}
