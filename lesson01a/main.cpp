#include <iostream>
#include <cstdlib>
// файл config.h будет получен по шаблону из файла config.h.in
#include <config.h>

int main(int argc, char* argv[])
{
  std::cout
    << "Hello World!" << std::endl
    << "Here you have " PROJECT_VERSION " version ;)" << std::endl;
  return EXIT_SUCCESS;
}
