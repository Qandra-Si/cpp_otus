// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <map>
#include <stdexcept>

#include <core/fake_heap_allocator.h>
#include <core/fake_heap_helpers.h>
#include <core/ringbuffer.h>


#ifdef NDEBUG
core::fake_heap_debugger_t* heap_debugger = nullptr;
#else
class heap_logger_t : public core::fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char>& get() { return std::cout; }
} heap_logger;
core::fake_heap_debugger_t* heap_debugger = &heap_logger;
#endif

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
  map_t::allocator_type a2(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 16+40*11, heap_debugger })); // msvc-x64: 8+24*11; gcc-x64: 16+40*11
  map_t m2(a2);

  // заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа
  for (int i = 0; i < 10; ++i) m2[i] = fact(i);

  // вывод на экран всех значений (ключ и значение разделены пробелом) хранящихся в контейнере
  for (int i = 0, cnt = m2.size(); i < cnt; ++i) std::cout << i << " " << m2[i] << std::endl;

  // создание экземпляра своего контейнера для хранения значений типа int
  using ring_t = core::ringbuffer_t<int, 11>;
  ring_t r3;

  // заполнение 10 элементами от 0 до 9
  for (int i = 0; i < 10; ++i) r3.push(i);

  // вывод на экран всех значений, хранящихся в контейнере
  for (const auto& ref : r3) std::cout << ref << " "; std::cout << std::endl;

  // создание экземпляра своего контейнера для хранения значений типа int с новым аллокатором, ограниченным 10 элементами
  using ring_alloc_t = core::fake_heap_allocator_t<int>;
  using fake_ring_t = core::ringbuffer_t<int, 11, ring_alloc_t>;
  fake_ring_t::allocator_type a4(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 8 + 4 * 11, heap_debugger }));
  fake_ring_t r4(a4);

  // заполнение 10 элементами от 0 до 9
  for (int i = 0; i < 10; ++i) r4.push(i);

  // вывод на экран всех значений, хранящихся в контейнере
  for (fake_ring_t::iterator itr = r4.begin(), end = r4.end(); itr != end; ++itr)
    std::cout << *itr << " ";
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
