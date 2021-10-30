// -*- mode: c++; coding: utf-8 -*-
#include <iostream>


/*! \brief Задача №1820. Уральские бифштексы
*
* https://acm.timus.ru/problem.aspx?space=1&num=1820
*
* После личного первенства довольные, но голодные программисты зашли в ресторан
* "Уральские бифштексы" и заказали себе n фирменных бифштексов. Для того чтобы
* приготовить бифштекс, повар должен прожарить каждую из его сторон на сковороде
* в течение одной минуты.
*
* К сожалению, у повара есть только одна сковорода, и на ней можно одновременно
* жарить не более k бифштексов. Определите, сколько минут понадобится повару,
* чтобы выполнить заказ программистов.
*
* Исходные данные: В единственной строке через пробел записаны целые числа
* n и k (1 <= n, k <= 1000).
*
* Результат: Выведите минимальное количество минут, за которое повар сможет
* приготовить n бифштексов.
*/
int main()
{
  int n, k;
  std::cin >> n >> k;
  // довольные, но голодные программист(ы) могут позволить себе целый 1 бифштекс
  // повар тоже чудик - владеет сковородой, в которой помещается 1 тыс. котлет
  if (k >= n)
  {
    // однако если программисты заказали 2 котлеты, а повар жарит по 100, то
    // пожарить за 1 минуту 4 стороны 2х котлет он не сможет
    std::cout << 2 << std::endl;
  }
  else
  {
    int number_of_pans = (int)(n / k);
    int more_steak_sides = (n % k) + n;
    number_of_pans += (more_steak_sides + k - 1) / k;
    std::cout << number_of_pans << std::endl;
  }
  return 0;
}

