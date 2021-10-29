// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <vector>


/*! \brief Задача №1000. A+B Problem
*
* https://acm.timus.ru/problem.aspx?space=1&num=1000
*
* Исходные данные: a и b
*
* Результат: a+b
*/
int main()
{
  int64_t a, b; // в условии не сказано какой разрядности числа, и не сказано вещественные ли они?
  std::cin >> a >> b;
  std::cout << a + b << std::endl;
  return 0;
}

