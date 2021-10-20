#include <iostream>
#include <cstdlib>
// файл cpp_otus_version.h будет получен по шаблону из файла cpp_otus_version.h.template
#include <cpp_otus_version.h>

int main(int argc, char* argv[])
{
  std::cout
    << "Hello World!" << std::endl
    << "Here you have " PROJECT_VERSION " version ;)" << std::endl;
  return EXIT_SUCCESS;
}
