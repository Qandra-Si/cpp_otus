// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <iomanip>
#include <cmath>


struct point_t
{
  double x;
  double y;
};


/*! \brief Задача №1020. Ниточка
*
* https://acm.timus.ru/problem.aspx?space=1&num=1020
*
* Злоумышленники варварски вбили в ни в чем не повинную плоскую поверхность N
* гвоздей, да так, что только шляпки остались. Мало того, они в своих подлых
* целях вбили все гвозди в вершины выпуклого многоугольника. После этого они...
* страшно сказать... они натянули ниточку вокруг всех гвоздей, так, что
* поверхности стало совсем больно! Вот как примерно они это сделали:
*
* (рисунок)
*
* Ваша задача — определить длину этой ниточки.
*
* Исходные данные: В первой строке входа к этой задаче находятся два числа —
* количество гвоздей N, 1 ≤ N ≤ 16, и вещественное число R — радиус шляпок
* гвоздей. Все шляпки имеют одинаковый радиус. Далее на входе располагаются еще
* N строк, в каждой из которых записана через пробел пара вещественных координат
* центра очередного гвоздя; координаты не превосходят по абсолютной величине
* числа 100. Описания гвоздей приводятся в порядке обхода вершин многоугольника
* (либо по часовой стрелке, либо против часовой стрелки), начиная с
* произвольного. Шляпки разных гвоздей не накладываются друг на друга.
*
* Результат: Выведите вещественное число, округлённое до двух знаков после
* запятой — длину ниточки, натянутой вокруг всех гвоздей.
*/
int main()
{
  int n;
  double r;
  std::cin >> n >> r;

  // считаем длину окружности
  const double circumference = 2 * 3.141592654 * r;
  if (n == 1)
  {
    std::cout << std::fixed << std::setprecision(2) << circumference << std::endl;
    return 0;
  }

  // считаем длину нити между центрами гвоздей
  double distance = 0.0;
  point_t prev, start;
  for (int i = 0, ii = n - 1; i <= ii; ++i)
  {
    point_t curr;
    std::cin >> curr.x >> curr.y;
    if (i == 0)
      start = curr;
    else
    {
      distance += sqrt(pow(prev.x - curr.x, 2.0) + pow(prev.y - curr.y, 2));
      if (i == ii)
        distance += sqrt(pow(start.x - curr.x, 2.0) + pow(start.y - curr.y, 2));
    }
    prev = curr;
  }
  // увеличиваем длину нити на полную длину окружности - как сумму сегментов,
  // которые огибает нить
  distance += circumference;
  std::cout << std::fixed << std::setprecision(2) << distance << std::endl;

  return 0;
}