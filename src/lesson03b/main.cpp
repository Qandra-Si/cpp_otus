// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>


// decltype(auto) - добавлено в C++14
// lambda closude initializers - добавлено в C++14
int main(int argc, char* argv[])
{
  char char_vars[5] = { 'a', 'b', 'c', 'd', '\0' };
  char * char_ptr_var = char_vars;
  decltype(auto) char_ptr_var2 = char_ptr_var; // в C++11 компилятор выдаст error: expected primary-expression before ‘auto’

  std::cout << char_vars << " " << *char_ptr_var << " " << char_ptr_var2 << std::endl; // abcd a abcd
  char_ptr_var++;
  std::cout << char_vars << " " << *char_ptr_var << " " << char_ptr_var2 << std::endl; // abcd b abcd
  char_ptr_var2++;  
  std::cout << char_vars << " " << *char_ptr_var << " " << char_ptr_var2 << std::endl; // abcd b bcd

  // ---

  auto closure_cpp14 = [var1=100](int var2, int var3) { return var1 + var2 + var3; }; // warning: lambda capture initializers only available with ‘-std=c++14’ or ‘-std=gnu++14’ 

  std::cout << closure_cpp14(5, 6) << std::endl;
  
  return EXIT_SUCCESS;
}
