// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <typeinfo>


int foo()
{
  return 5;
}

class class_t
{
public:
  int a;
  class_t() : a(-1) {}
};

auto kuk(int a, float b) -> float // error: ‘kuk’ function uses ‘auto’ type specifier without trailing return type
{                                 // note: deduced return type only available with ‘-std=c++14’ or ‘-std=gnu++14’
  if (a)
    return a;
  else
    return b;
}

template<typename T>
void template_func(T)
{
}

// автоматическое выведение типов
int main(int argc, char* argv[])
{
  auto foo_res = foo();
  int int_var = 1;
  float float_var = 1.0;
  class_t class_var;
  auto class_var2 = class_t();
  auto const_char_ptr = "Hello, World!";
  auto kuk_var1 = kuk(0, 1.0);
  auto kuk_var2 = kuk(1, 1.0);

  std::cout << typeid(foo_res).name() << std::endl;
  std::cout << typeid(int_var).name() << std::endl;
  std::cout << typeid(float_var).name() << std::endl;
  std::cout << typeid(class_var).name() << std::endl;
  std::cout << typeid(class_var2).name() << std::endl;
  std::cout << typeid(const_char_ptr).name() << std::endl;
  std::cout << typeid(kuk_var1).name() << std::endl;
  std::cout << typeid(kuk_var2).name() << std::endl;

  // из типа инициализирующего значения шаблона убирается const, reference, volatile
  int arr[3] = {0, 2, 4};
  auto arr_3_non_ref_var = arr[2];
  auto & arr_3_ref_var = arr[2];
  
  std::cout << typeid(arr_3_non_ref_var).name() << std::endl;
  std::cout << arr[2] << " " << arr_3_non_ref_var << " " << arr_3_ref_var << std::endl;
  arr_3_non_ref_var = 6; // это не reference
  std::cout << arr[2] << " " << arr_3_non_ref_var << std::endl;
  arr_3_ref_var = 6; // это reference
  std::cout << arr[2] << " " << arr_3_ref_var << std::endl;

  const int const_var = 42;
  auto non_const_var = const_var;
  const auto const_var2 = const_var;

  std::cout << const_var << " " << non_const_var << std::endl;
  non_const_var = 6; // это не const
  //const_var2 = 6; // error: assignment of read-only variable ‘const_var2’
  std::cout << const_var << " " << non_const_var << std::endl;

  volatile int volatile_var = 42;
  auto non_volatile_var = volatile_var;

  std::cout << typeid(volatile_var).name() << std::endl;
  std::cout << typeid(non_volatile_var).name() << std::endl;

  int int_vars[2] = { 55, 56 };
  int int_var2 = 33;
  
  const int * const_ptr_var = int_vars;
  auto const_ptr_var2 = const_ptr_var;
  
  std::cout << typeid(const_ptr_var).name() << std::endl;
  std::cout << *const_ptr_var << " " << *const_ptr_var2 << std::endl; // 55 55
  const_ptr_var++;
  const_ptr_var2++;
  std::cout << *const_ptr_var << " " << *const_ptr_var2 << std::endl; // 56 56
  //*const_ptr_var2 = 6; // error: assignment of read-only location ‘* const_ptr_var2’
  const_ptr_var2 = &int_var2; // error: assignment of read-only location ‘* ptr_const_var2’
  std::cout << *const_ptr_var << " " << *const_ptr_var2 << std::endl; // 56 33

  int * const ptr_const_var = int_vars;
  auto ptr_const_var2 = ptr_const_var;

  std::cout << typeid(ptr_const_var).name() << std::endl;
  std::cout << *ptr_const_var << " " << *ptr_const_var2 << std::endl; // 55 55
  //ptr_const_var++; // error: increment of read-only variable ‘ptr_const_var’
  ptr_const_var2++;
  std::cout << *ptr_const_var << " " << *ptr_const_var2 << std::endl; // 55 56
  *ptr_const_var = 6;
  *ptr_const_var2 = 6; // error: assignment of read-only location ‘* ptr_const_var2’
  std::cout << *ptr_const_var << " " << *ptr_const_var2 << std::endl; // 6 6
  //ptr_const_var = &int_var2; // error: assignment of read-only variable ‘ptr_const_var’  
  ptr_const_var2 = &int_var2;
  std::cout << *ptr_const_var << " " << *ptr_const_var2 << std::endl; // 6 33

  return EXIT_SUCCESS;
}
