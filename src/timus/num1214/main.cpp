// -*- mode: c++; coding: utf-8 -*-
#include <iostream>


/*! \brief Задача №1214. Странная процедура
*
* https://acm.timus.ru/problem.aspx?space=1&num=1214
*
* В программировании иногда требуется тестировать и отлаживать уже существующий
* программный код. Представьте себе, что ваш коллега передал вам написанный им
* фрагмент кода, так как должен заняться разработкой другой программы.
*
* Вот этот фрагмент кода — процедура с двумя параметрами:
\code
void P(int x, int y)
{
  if (x > 0 && y > 0)
  {
    for (int i = 0; i < x + y; i++)
    {
      y = x * x + y;
      x = x * x + y;
      y = sqrt(x + y / abs(y) * -abs(y));
      for (int j = 0; j < 2 * y; j++)
        x -= y;
    }
  }
  printf"%d %d\n", x, y);
}
\endcode
*
* Ваша задача несколько необычна: для отладки этой процедуры необходимо
* разработать программу, которая по данным, выведенным на экран, восстановит
* входные параметры процедуры. Гарантируется, что во время работы процедуры ни
* одна из переменных не выходила за пределы своего типа.
*
* Исходные данные: В единственной строке записаны целые числа x и y, выведенные
* на экран в конце работы процедуры (−32000 <= x, y <= 32000).
*
* Результат: Выведите целые числа x и y, которые были поданы процедуре на вход.
*/
int main()
{
  int x, y;
  std::cin >> x >> y;

  // с первым условием всё понятно, если числа <= 0, то они сами и являются ответом
  if (x <= 0 || y <= 0)
  {
    std::cout << x << " " << y << std::endl;
    return 0;
  }
  // а вот дальше... упрощаем тело цикла:
/*
for (int i = 0; i < x + y; i++)
{
  y1 = x * x + y
  x1 = x * x + y1
  y2 = sqrt(x1 + y1/abs(y1) * -abs(y1))
  for (int j = 0; j < 2*y2; j++)
    x1 -= y2
  x = x1
  y = y2
}
---
y1 = x*x + y
x1 = x*x + x*x + y = 2*x*x + y
y2 = sqrt(2*x*x + y + (x*x+y)/(x*x+y) * -(x*x+y)) = sqrt(2*x*x + y -x*x - y) = sqrt(x*x) = x
x1 = x1 - y2*2*y2 = 2*x*x + y - x*2*x = y
x,y = y,x
---
(i < x+y) == (i < y+x)
*/
  if ((x+y)%2)
    std::cout << y << " " << x << std::endl;
  else
    std::cout << x << " " << y << std::endl;

  return 0;
}
