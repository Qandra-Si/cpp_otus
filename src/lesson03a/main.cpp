// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>


int foo(int a, int b)
{
  std::cout << "Silence is golden" << std::endl; // этот код не вызывается
  return a + b;
}

template<typename T, typename R>
auto template_func(T t, R r) -> decltype(t * r)
{
  return t * r;
}

template<typename T, typename R>
auto template_func2(T t, R r) -> decltype(t)
{
  return t * r;
}

// decltype
int main(int argc, char* argv[])
{
  auto int_var = 42;
  decltype(int_var) int_var2 = 3;

  std::cout << int_var << " " << int_var2 << std::endl; // 42 3

  // ---
  
  const int const_var = 43;
  decltype(const_var) const_var2 = 3;

  std::cout << const_var << " " << const_var2 << std::endl; // 42 3
  // const_var2 = 4; // error: assignment of read-only variable ‘const_var2’
  std::cout << const_var << " " << const_var2 << std::endl; // 42 3

  // ---
  
  const int * const_ptr = &int_var;
  decltype(const_ptr) const_ptr2 = &int_var;

  std::cout << *const_ptr << " " << *const_ptr2 << std::endl; // 42 42
  const_ptr2 = &int_var2;
  // *const_ptr2 = 6; // error: assignment of read-only location ‘* const_ptr2’
  std::cout << *const_ptr << " " << *const_ptr2 << std::endl; // 42 3

  // ---
  
  int * const ptr_const = &int_var;
  decltype(ptr_const) ptr_const2 = &int_var;

  std::cout << *ptr_const << " " << *ptr_const2 << std::endl; // 42 42
  //ptr_const2 = &int_var2; // error: assignment of read-only variable ‘ptr_const2’
  //ptr_const2++; // error: increment of read-only variable ‘ptr_const2’
  *ptr_const2 = 3;
  std::cout << *ptr_const << " " << *ptr_const2 << std::endl; // 3 3

  // ---

  decltype(foo(1, 2)) foo_var = 54;
  std::cout << foo_var << std::endl; // 54
  
  // ---

  int * ptr_var3 = nullptr;
  decltype(ptr_var3) ptr_var4 = &int_var;
  decltype(*ptr_var3) ptr_var5 = int_var;

  std::cout << /**ptr_var3 << " " <<*/ *ptr_var4 << " " << ptr_var5 << std::endl; // 3 3

  // ---

  char char_var = 'a', one_more_char_var = 'z';
  decltype(char_var) char_var2 = 'b';
  //decltype( (char_var) ) char_var3 = 'c'; // rror: cannot bind non-const lvalue reference of type ‘char&’ to an rvalue of type ‘char’
  decltype( (char_var) ) char_var4 = one_more_char_var;

  std::cout << char_var << " " << char_var2 << " " << char_var4 << " " << one_more_char_var << std::endl; // a b z z
  char_var2 = 's';
  char_var4 = 'x';
  std::cout << char_var << " " << char_var2 << " " << char_var4 << " " << one_more_char_var << std::endl; // a s x x

  // ---

  std::cout << template_func(2, 3) << std::endl; // 6
  std::cout << template_func(1.5, 1.7) << std::endl; // 2.55
  std::cout << template_func((uint8_t)0xff, (uint8_t)2) << std::endl; // 510
  std::cout << template_func('a', 'b') << std::endl; // 9506
  
  std::cout << template_func2(2, 3) << std::endl; // 6
  std::cout << template_func2(1.5, 1.7) << std::endl; // 2.55
  std::cout << (int)template_func2((uint8_t)0xff, (uint8_t)2) << std::endl; // 254
  std::cout << template_func2('a', 'b') << std::endl; // "
  
  return EXIT_SUCCESS;
}
