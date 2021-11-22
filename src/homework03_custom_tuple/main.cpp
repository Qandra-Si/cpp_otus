// -*- mode: c++; coding: utf-8 -*-

#include <string>
#include <tuple>
#include <cassert>

// задача "реализовать" custom_tuple и custom_tie без использования variadic templates
// свелась к копированию элементов custom_tuple в custom_tie

template<typename T1, typename T2, typename T3, typename T4>
struct custom_tuple_t
{
  const T1 t1;
  const T2 t2;
  const T3 t3;
  const T4 t4;
  custom_tuple_t(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4) : t1(t1), t2(t2), t3(t3), t4(t4) { }
  custom_tuple_t(const custom_tuple_t & src) : t1(src.t1), t2(src.t2), t3(src.t3), t4(src.t4) { }
};

typedef custom_tuple_t<std::string, std::string, std::size_t, std::string> custom_tuple;

template<typename T1, typename T2, typename T3, typename T4>
struct custom_tie_t
{
  T1 & a;
  T2 & b;
  T3 & c;
  T4 & d;
  custom_tie_t(T1& a, T2& b, T3& c, T4& d) : a(a), b(b), c(c), d(d) { }
  void operator=(const custom_tuple_t<T1,T2,T3,T4> & src)
  {
    a = src.t1;
    b = src.t2;
    c = src.t3;
    d = src.t4;
  }
};

typedef custom_tie_t<std::string, std::string, std::size_t, std::string> custom_tie;

custom_tuple getPerson()
{
  const std::string name = "Petia";
  const std::string secondName = "Ivanoff";
  const std::size_t age = 23;
  const std::string department = "Sale";
  return custom_tuple(name, secondName, age, department);
}

int main(int argc, char * argv[])
{
  std::string name, secondName, department;
  std::size_t age;

  custom_tuple("Petia", "Ivanoff", 23, "Sale");

  custom_tie(name, secondName, age, department) = getPerson();

  assert(name == "Petia");
  assert(secondName == "Ivanoff");
  assert(age == 23);
  assert(department == "Sale");

  return 0;
}
