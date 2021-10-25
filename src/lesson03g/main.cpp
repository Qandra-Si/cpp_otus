// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <tuple>
#include <functional>


//auto foo(int, int) // deduced return type only available with ‘-std=c++14’ or ‘-std=gnu++14'
std::tuple<std::string, int> foo(int, int a) // c++11
{
  return std::make_tuple(std::string("value"), a);
}

// tuple
int main(int argc, char* argv[])
{
  auto multi_output = foo(5, 6);

  std::cout << std::get<0>(multi_output) << std::endl;
  std::cout << std::get<1>(multi_output) << std::endl;

  std::get<1>(multi_output) = 33; // можно менять значение tuple, на стриме была ошибка <0>

  std::cout << std::get<1>(multi_output) << std::endl;
  
  std::string str_ref;
  int int_ref;

  std::make_tuple(std::ref(str_ref), std::ref(int_ref)) = foo(0, 0);

  std::cout << str_ref << std::endl;
  std::cout << int_ref << std::endl;

  std::tie(str_ref, int_ref) = foo(100, 200);

  std::cout << str_ref << std::endl;
  std::cout << int_ref << std::endl;
  
  return EXIT_SUCCESS;
}
