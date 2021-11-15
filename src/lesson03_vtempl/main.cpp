#include <iostream>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__


void example1();
void example2();
void example3();

int main()
{
  example1();
  // iterate1: sizeof = 3 values = 1 3.140000 'Hello, World!'

  example2();
  /*
  void iterate2(T, Args ...) [with T = int; Args = {double, const char*}]
  1
  void iterate2(T, Args ...) [with T = double; Args = {const char*}]
  3.14
  void iterate2(T, Args ...) [with T = const char*; Args = {}]
  Hello, World!
  void iterate2()
  */

  example3();
  // 13.14Hello, World!
  return 0;
}

template<typename... Args>
  void iterate1(Args... values)
  {
    printf("iterate1: sizeof = %lu values = %d %f '%s'\n",
           sizeof...(values), values...);
  }

void example1()
{
  iterate1(1, 3.14, "Hello, World!");
}

void iterate2()
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template<typename T, typename... Args>
  void iterate2(T v, Args... args)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout << v << std::endl;
    iterate2(args...);
  }

void example2()
{
  iterate2(1, 3.14, "Hello, World!");
}

template<typename... Args>
  void iterate3(Args... args)
  {
    int a[sizeof...(args)] = {(std::cout << args, 0)...};
    std::cout << std::endl;
  }

void example3()
{
  iterate3(1, 3.14, "Hello, World!");

  // скобки и оператор запятая выбирают последний элемент 40
  std::cout << (1 , 40) << std::endl;
  // скобки и оператор запятая выбирают последний элемент 100
  std::cout << ("1" , 40, 100) << std::endl;
}

