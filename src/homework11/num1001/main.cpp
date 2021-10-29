// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <vector>


/*! \brief Задача №1001. Обратный корень
*
* https://acm.timus.ru/problem.aspx?space=1&num=1001
*
* Исходные данные: Входной поток содержит набор целых чисел Ai (0 <= Ai <= 10^18), отделённых друг от друга
* произвольным количеством пробелов и переводов строк. Размер входного потока не превышает 256 КБ.
*
* Результат: Для каждого числа Ai, начиная с последнего и заканчивая первым, в отдельной строке вывести
* его квадратный корень не менее чем с четырьмя знаками после десятичной точки.
*/
int main()
{
  uint64_t input;
  std::vector<double> output;
  output.reserve(256*1024/2);
  
  while (std::cin >> input)
  {
    output.push_back(std::sqrt((double)input));
  }

  std::cout.precision(4);
  std::cout << std::fixed;
  for (std::vector<double>::reverse_iterator itr = output.rbegin(), end = output.rend(); itr != end; ++itr)
  {
    std::cout << *itr << std::endl;
  }

  return 0;
}

