// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <vector>
#include <algorithm>


typedef std::vector<unsigned int> numbers_t;

int get_own_numbers(const numbers_t & own, const numbers_t & their1, const numbers_t & their2);

/*! \brief Задача №1880. Собственные числа Psych Up
*
* https://acm.timus.ru/problem.aspx?space=1&num=1880
*
* Шёл очередной контест Петрозаводских сборов. Игроки команды Psych Up быстро
* нашли простую задачу, и Федя сел за компьютер. Через пять минут решение было
* готово. Не тратя времени на тестирование, Федя отправил его на проверку и
* через несколько секунд получил вердикт Time Limit Exceeded.
*
* Федя скомкал условие задачи и вышел из класса, хлопнув дверью. Что-то шло не
* так, нужно было развеяться. По пути в туалет он услышал разговор авторов
* контеста. Паша обсуждал со своим другом решение той самой задачи. Федя смог
* разобрать из их разговора только слова "собственные числа".
*
* Федя подумал и решил, что у него, конечно же, есть собственные числа.
* Например, дата рождения, номер квартиры, оценка на последнем экзамене,
* количество поездок на соревнования. Но ведь контест командный. А что такое
* собственные числа команды? Естественно, что число является собственным для
* команды, если оно собственное для каждого из её участников. С такими
* радостными мыслями Федя направился назад в аудиторию.
*
* Исходные данные: Входные данные состоят из трёх блоков по две строки. Первая
* строка каждого блока содержит целое число n —количество собственных чисел
* очередного игрока (1 <= n <= 4 000). Во второй строке блока записано n целых
* различных чисел в порядке возрастания — собственные числа очередного игрока.
* Все собственные числа — целые, положительные и не превосходят 109.
*
* Результат: Выведите количество собственных чисел команды Psych Up.
*/
int main()
{
  numbers_t numbers[3];
  for (int i = 0; i < 3; ++i)
  {
    int n;
    std::cin >> n;
    numbers[i].resize(n);
    for (int j = 0; j < n; ++j)
      std::cin >> numbers[i][j];
  }
  // Федя - мальчик странный, он сначала решил задачу, а потом только узнал её
  // условие.
  int own_numbers = 0;
  for (int i = 0; i < 3; ++i)
  {
    own_numbers += get_own_numbers(numbers[i], numbers[(i+1)%3], numbers[(i+2)%3]);
  }
  std::cout << own_numbers << std::endl;
  return 0;
}

int get_own_numbers(const numbers_t & own, const numbers_t & their1, const numbers_t & their2)
{
  int res = 0;
  const numbers_t::const_iterator their1_begin = their1.begin();
  const numbers_t::const_iterator their1_end = their1.end();
  const numbers_t::const_iterator their2_begin = their2.begin();
  const numbers_t::const_iterator their2_end = their2.end();
  for (const auto & num : own)
  {
    if (std::find(their1_begin, their1_end, num) != their1_end) continue;
    if (std::find(their2_begin, their2_end, num) != their2_end) continue;
    res++;
  }
  return res;
}
