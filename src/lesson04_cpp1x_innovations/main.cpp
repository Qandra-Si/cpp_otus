#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <vector>
#include <list>
#include <cassert>
#include <array>
#include <cstdint>
#include <memory>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__



void example1(); // битовые константы
void example2(); // оператор с суффиксом
void example3(); // chrono
void example4(); // using
void example5(); // assert
void example6(); // constexpr
void example7(); // nullptr_t
void example8(); // braced initialization
void example9(); // enum class
void exampleA(); // новые ключевые слова для описания классов

int main()
{
  example1();
  example2();
  example3();
  example4();
  example5();
  example6();
  example7();
  example8();
  example9();
  exampleA();
  return 0;
}

void example1()
{
  std::cout << "example1:" << std::endl;
  unsigned int a = 0b10100101; // 165
  std::cout << a << std::endl;
  unsigned int b = 0b11111111111111111111111111111111ul; // 4294967295
  std::cout << b << std::endl;
  int c = 0b11111111111111111111111111111111; // -1
  std::cout << c << std::endl;
}

struct Hz
{
  unsigned long long val;
  unsigned long long get() const { return val; }
};

Hz operator""_Hz(unsigned long long val)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return Hz{val};
}

struct MHz
{
  long double val;
  unsigned long long hz() const { return (unsigned long long)(0.5 + val * 1000000000); }
  long double mhz() const { return val; }
};

MHz operator""_MHz(long double val)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return MHz{val};
}

// warning: literal operator suffixes not preceded by ‘_’ are reserved for future standardization [-Wliteral-suffix]
std::string operator""_lower(const char * p, size_t sz)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::string res(p, p + sz);
  std::for_each(res.begin(), res.end(), [](char & c) { c = ::tolower(c); });
  return res;
}

void example2()
{
  std::cout << "example2:" << std::endl;

  auto gsm = 900000000_Hz; // -> operator""_Hz(900000000);
  auto dcs = 1900000000_Hz; // -> operator""_Hz(1900000000);
  std::cout << gsm.get() << " " << dcs.get() << std::endl;

  auto wifi = 2.4_MHz; // -> operator""_MHz(2400);
  std::cout << wifi.mhz() << " " << wifi.hz() << std::endl;

  //std::cout << 850.0_MHz.mhz() << std::endl; // error: unable to find numeric literal operator ‘operator""_MHz.mhz’
  std::cout << 850.0_MHz .mhz() << std::endl;

  std::cout << "Hello, World!"_lower << std::endl;
}

void example3()
{
  std::cout << "example3:" << std::endl;

  using namespace std::chrono_literals;

  auto s = std::chrono::seconds(61);
  std::cout << s.count() << std::endl;

  auto seconds = 90s; // суффикс s аналогичен std::chrono::seconds
  std::cout << seconds.count() << std::endl;
}

typedef std::vector<int> my_vector_t;

using my_list_t = std::list<int>;

template<typename T>
using v = std::vector<T>; // здесь нельзя пользоваться typedef

void example4()
{
  std::cout << "example4:" << std::endl;

  my_list_t lst;
  v<int> int_v;
  v<char> char_v;
}

void example5()
{
  std::cout << "example5:" << std::endl;

  int val;
  std::cout << "Input int value (static_assert(value==0, 'WOW!'): ";
  std::cin >> val;

  assert(val == 0);
  std::cout << "OK, val=" << val << std::endl;

  const int lib_version = 5;
  static_assert(lib_version == 5, "failed");
}

constexpr int sz_foo(int value)
{
  return value * 2;
}

struct KHz
{
  constexpr KHz(unsigned khz) : hz(khz * 1000) {}
  constexpr unsigned get() const { return hz / 1000; }
private:
  unsigned hz;
};

void example6()
{
  std::cout << "example6:" << std::endl;

  const int sz = 2; // должно присутствовать ключевое слово const
  int buff[sz]; // C4101
  static_assert(sz < 3, "too big buffer!");

  constexpr int sz2 = sz_foo(2);
  int buff2[sz2] = {0};
  static_assert(sz2 < 30, "too big buffer!");
  std::array<int, sz2> arr2;

  ((void)(buff2));
  ((void)(arr2));

  int a;
  std::cout << "Input int value: ";
  std::cin >> a;
  std::cout << "sz_foo(a) = " << sz_foo(a) << std::endl; // здесь будет скомпилирована вторая версия функции sz_foo

  constexpr KHz khz{2400};
  static_assert(khz.get() >= 2000, "low freq band");

  constexpr int x = 33;
  int y = x;
  //constexpr int z = y; // error: the value of ‘y’ is not usable in a constant expression
}

void foo7(const int *) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
void foo7(const char *) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
void foo7(std::nullptr_t) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

void example7()
{
  std::cout << "example7:" << std::endl;

  foo7(nullptr);
}

void print_buff(const std::vector<int> & v)
{
  std::for_each(v.begin(), v.end(), [](const int & i) { std::cout << i << " "; });
  std::cout << std::endl;
}

struct brace_t
{
  brace_t() = default;
  brace_t(int a, int b) : a(a * 10), b(b * 10), c(33) { }
  brace_t(std::initializer_list<int> il) : a(il.size()), b(0), c(0) { }

  int a{-1};
  int b{-1};
  int c{-1};
};

void print_brace(const brace_t & b)
{
  std::cout << b.a << " " << b.b << " " << b.c << std::endl;
}

void example8()
{
  std::cout << "example8:" << std::endl;

  std::vector<int> buff1(10, 0);
  print_buff(buff1);

  std::vector<int> buff2{10, 0}; // std::vector<std::initializer_list<int>{10, 0}>
  print_buff(buff2);

  brace_t b1(1, 2);
  print_brace(b1);

  brace_t b2{1, 2};
  print_brace(b2);
}

enum symbols1_t { a, b, c };
//enum symbols2_t { a, b, c }; // error: ‘a’ conflicts with a previous declaration

enum class states_t : uint8_t {
  enabled = 1,
  disabled = 0,
  unknown = 127
};
enum class stream_t : uint16_t {
  enabled = 1,
  disabled = 0,
};

void example9()
{
  std::cout << "example9:" << std::endl;

  //states_t x = unknown; // error: ‘unknown’ was not declared in this scope; did you mean ‘states_t::unknown’?
  states_t a = states_t::unknown;
  stream_t b = stream_t::disabled;
  //std::cout << states_t::enabled << std::endl;
  std::cout << static_cast<int>(a) << " " << sizeof(a) << std::endl;
  std::cout << static_cast<int>(b) << " " << sizeof(b) << std::endl;
}

struct foo_t
{
  foo_t() = delete;
  foo_t(int) { }

  virtual ~foo_t() = default;

  foo_t & operator=(const foo_t & rhs) = default;

  virtual void m() = 0;
  virtual void n(int, double) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

struct bar_t : foo_t
{
  bar_t(int value) : foo_t(value) { }
  virtual void m() { }
  void n(int, double) override final { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

struct kuk_t : bar_t
{
  // метод n перестал быть виртуальным из-за ключевого слова final в bar_t
  //void n(int, double) override; // error: virtual function ‘virtual void kuk_t::n(int, double)’ overriding final function
  //void n(int, double) final; // error: virtual function ‘virtual void kuk_t::n(int, double)’ overriding final function
};

void exampleA()
{
  std::cout << "exampleA:" << std::endl;

  //foo_t a; // error: use of deleted function ‘foo_t::foo_t()’
  bar_t b(33);
  b.n(5, 8.0);

  std::unique_ptr<foo_t> c = std::make_unique<bar_t>(42);
  c->n(1, 3.14);
}
