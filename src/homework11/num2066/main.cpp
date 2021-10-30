// -*- mode: c++; coding: utf-8 -*-
#include <iostream>


/*! \brief Задача №2066. Простое выражение
*
* https://acm.timus.ru/problem.aspx?space=1&num=2066
*
* Вы наверняка уже знаете, что Алексей — серьёзный математик, и он любит решать
* серьёзные задачи. Вот ещё одна из этих задач.
*
* Даны три неотрицательных числа a, b, c. Нужно расставить их в некотором
* порядке и поставить между ними знаки +, − или * таким образом, чтобы результат
* получившегося арифметического выражения был минимален. Между каждой парой
* соседних чисел должен быть один знак, унарный минус и скобки использовать
* нельзя. Приоритет операций стандартный (сначала умножение).
*
* Исходные данные: Даны целые неотрицательные числа a, b, c в неубывающем
* порядке (0 <= a <= b <= c <= 100), каждое в отдельной строке.
*
* Результат: Выведите одно целое число — минимальное значение выражения.
*/
int main()
{
  int a, b, c;
  std::cin >> a >> b >> c;
  if (b == 0) // то и a == 0, значит ответ будет -c
    std::cout << -c << std::endl;
  else if (b == 1) // то a in (0, 1), тогда минимум будет достигнут с a-b-c
    std::cout << a - b - c << std::endl;
  else // во всех остальных случаях a - b * c
    std::cout << a - b * c << std::endl;
  return 0;
}

