#include <iostream>
#include <cstdlib>
// файл cpp_otus_config.h будет получен по шаблону из файла cpp_otus_config.h.template
#include <cpp_otus_config.h>

int main(int argc, char* argv[])
{
  std::cout
    << "Hello World!" << std::endl
    << "Here you have " PROJECT_VERSION " version ;)" << std::endl;
  return EXIT_SUCCESS;
}
