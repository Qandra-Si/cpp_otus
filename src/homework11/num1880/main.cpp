// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <set>
#include <algorithm>


typedef std::set<unsigned int> numbers_t;

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
    for (int j = 0; j < n; ++j)
    {
      numbers_t::value_type ni;
      std::cin >> ni;
      numbers[i].insert(ni);
    }
  }
  // Федя - мальчик странный, он сначала "решил" задачу, а потом только узнал её
  // условие.
  // П.С.: Я тоже попался в эту ловушку и стало понятно почему название задачи
  //       именно такое. Надо искать не уникальные числа у участников команды, а
  //       числа, которые есть у каждого из участников
  numbers_t same_01_numbers, same_01_2_numbers;
  std::set_intersection(
    numbers[0].begin(), numbers[0].end(),
    numbers[1].begin(), numbers[1].end(),
    std::inserter(same_01_numbers, same_01_numbers.begin())
  );
  std::set_intersection(
    numbers[2].begin(), numbers[2].end(),
    same_01_numbers.begin(), same_01_numbers.end(),
    std::inserter(same_01_2_numbers, same_01_2_numbers.begin())
  );
  std::cout << same_01_2_numbers.size() << std::endl;
  return 0;
}
