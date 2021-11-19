// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <map>
#include <stdexcept>

#include <core/fake_heap_allocator.h>
#include <core/fake_heap_helpers.h>


class heap_logger_t : public core::fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char>& get() { return std::cout; }
} heap_logger;

constexpr int fact(int n)
{
  if (n < 0) throw std::invalid_argument("Факториал отрицательных чисел не существует");
  if (n <= 1) return 1;
  int res = n;
  while (n >= 2) res *= --n;
  return res;
}

int main()
{
  // создание экземпляра std::map<int, int>
  using general_map_t = std::map<int, int>;
  general_map_t m1;

  // заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
  for (int i = 0; i < 10; ++i) m1[i] = fact(i);

  // создание экземпляра std::map<int, int> с новым аллокатором, ограниченным 10 элементами
  using alloc_t = core::fake_heap_allocator_t<std::pair<const int, int>>;
  using map_t = std::map<int, int, std::less<int>, alloc_t>;
  map_t::allocator_type a(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 8+24*11, &heap_logger }));
  map_t m2(a);

  // заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
  for (int i = 0; i < 10; i++) m2[i] = fact(i);

  // вывод на экран всех значений (ключ и значение разделены пробелом) хранящихся в контейнере
  for (int i = 0, cnt = m2.size(); i < cnt; ++i) std::cout << i << " " << m2[i] << std::endl;

  // создание экземпляра своего контейнера для хранения значений типа int

  // заполнение 10 элементами от 0 до 9

  // создание экземпляра своего контейнера для хранения значений типа int с новым аллокатором, ограниченным 10 элементами

  // заполнение 10 элементами от 0 до 9

  // вывод на экран всех значений, хранящихся в контейнере

  return EXIT_SUCCESS;
}
