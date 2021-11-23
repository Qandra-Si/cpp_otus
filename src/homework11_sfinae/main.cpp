// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <limits>
#include <type_traits>
#include <vector>
#include <list>
#include <tuple>


// ---------- ---------- ---------- ---------- ----------
// вывод простых типов на экран
// ---------- ---------- ---------- ---------- ----------

template<typename T>
void print_ip(T value) {
  if constexpr (std::numeric_limits<T>::is_integer)
  {
    using cv_type = typename std::remove_cv<T>::type;
    using ucv_type = typename std::make_unsigned<cv_type>::type;
    constexpr unsigned sz = sizeof(value);
    ucv_type _value = static_cast<ucv_type>(value);
    // ничего не сказано, сколько октет может быть в "условном ip-адресе"...
    // но раз уж любая произвольная строка - норм, то но 1.2.3.4.5 тоже считаем нормой
    for (unsigned i = 0, shift = 8 * (sz-1); i < sz; ++i, shift -= 8)
    {
      if (i) std::cout << '.';
      std::cout << ((static_cast<ucv_type>(value) >> shift) & 0xffull);
    }
    std::cout << std::endl;
  }
  else if constexpr (std::is_same<const char*,T>::value || std::is_same<std::string,T>::value)
  {
    std::cout << value << std::endl;
  }
  else
  {
    assert(("Type deduction error on print_ip", false));
  }
}

// ---------- ---------- ---------- ---------- ----------
// вывод содержимого контейнеров на экран
// ---------- ---------- ---------- ---------- ----------

template<typename Cont>
void print_container_as_ip(Cont values) {
  // ничего не сказано, что содержит vector/list для "условного ip-адреса"...
  // но раз уж любая произвольная строка - норм, то но Hello.World тоже считаем нормой
  bool first = true;
  auto dot = [&first](){ if (!first) std::cout << '.'; first = false; };
  for (const auto & ref : values)
  {
    dot();
    std::cout << ref;
  }
  std::cout << std::endl;
}

template<typename U>
void print_ip(std::vector<U> values) {
  print_container_as_ip(values);
}
template<typename U>
void print_ip(std::list<U> values) {
  print_container_as_ip(values);
}

// ---------- ---------- ---------- ---------- ----------
// дополнительное задание с выводом std::tuple на экран
// ---------- ---------- ---------- ---------- ----------

template <int N, typename Tx>
void tuple_get_all(Tx x) {
  if constexpr (N > 0)
    tuple_get_all<N-1>(x);
  std::cout << (N?".":"") << std::get<N>(x);
}

template<class... Args>
void print_ip(std::tuple<Args... > values) {
  tuple_get_all<std::tuple_size<decltype(values)>::value-1>(values);
  std::cout << std::endl;
}

/*! \brief main
 *
 * Ожидается вывод на экран следующих результатов:
 *
\code

255
0.0
127.0.0.1
123.45.67.89.101.112.131.41
localhost
192.168.0.0/16
2001.0db8.85a3.0000.0000.8a2e.0370.7334
8.8.8.8
33.3.14.Hello.World.!

\endcode
 */
int main()
{
  print_ip(char(-1));
  print_ip(short(0));
  print_ip(int(2130706433));
  print_ip(long(8875824491850138409));
  print_ip("localhost");
  print_ip(std::string("192.168.0.0/16"));
  print_ip(std::vector<const char*>{"2001","0db8","85a3","0000","0000","8a2e","0370","7334"});
  print_ip(std::list<int>(4,8));
  print_ip(std::make_tuple(33, 3.14, "Hello.World", '!'));

  return EXIT_SUCCESS;
}
