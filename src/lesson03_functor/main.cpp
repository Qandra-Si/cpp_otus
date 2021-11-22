// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>


// functor
struct filter_t
{
  explicit filter_t(int val) : val(val) { }
  void operator()(int val)
  {
    if (val != this->val)
      std::cout << "filter" << val << std::endl;
  }
private:
  int val;
};

// замыкание
struct closure_t
{
  explicit closure_t(int a) : a(a) { }
  int operator()(int x, int y)
  {
    return x + y + a;
  }
private:
  int a;
};

int main(int argc, char* argv[])
{
  filter_t filter_instance(5); // экземпляр функтора

  filter_instance(6); // filter6
  filter_instance(5);

  std::vector<int> values;
  values.push_back(5);
  values.push_back(1);
  values.push_back(2);
  
  std::for_each(values.begin(), values.end(), filter_t(1)); // filter5 filter2

  // ---

  closure_t closure_instance(100);
  std::cout << closure_instance(5, 6) << std::endl; // 111
  std::cout << closure_instance(10, 18) << std::endl; // 128
  std::cout << closure_t(100)(-1, 0) << std::endl; // 99

  // auto closure_cpp14 = [var1=100](int var2, int var3) { return var1 + var2 + var3; }; // warning: lambda capture initializers only available with ‘-std=c++14’ or ‘-std=gnu++14’ 

  // лямбда - получаем функцинональность функторов, без необходимости описания самого функтора 
  // смотрим результат трансляции, где компилятор за нас генерирует функтор https://cppinsights.io/s/6240ee7c
  int var1 = 100;
  auto closure_cpp11 = [var1](int var2, int var3) { return var1 + var2 + var3; };

  std::cout << closure_cpp11(-10, -20) << std::endl; // 70
  
  return EXIT_SUCCESS;
}
