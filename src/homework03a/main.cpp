// -*- mode: c++; coding: utf-8 -*-

#include <string>
#include <tuple>
#include <cassert>

//auto getPerson() // deduced return type only available with ‘-std=c++14’ or ‘-std=gnu++14'
std::tuple<std::string, std::string, std::size_t, std::string> getPerson() // c++11
{
  const std::string name = "Petia";
  const std::string secondName = "Ivanoff";
  const std::size_t age = 23;
  const std::string department = "Sale";
  return std::make_tuple(name, secondName, age, department);
}

template<typename T1, typename T2, typename T3, typename T4>
struct custom_tie_t
{
  T1 & a;
  T2 & b;
  T3 & c;
  T4 & d;
  custom_tie_t(T1& a, T2& b, T3& c, T4& d) : a(a), b(b), c(c), d(d) { }
  void operator=(const std::tuple<T1, T2, T3, T4> & src)
  {
    a = std::get<0>(src);
    b = std::get<1>(src);
    c = std::get<2>(src);
    d = std::get<3>(src);
  }
};

typedef custom_tie_t<std::string, std::string, std::size_t, std::string> custom_tie;

int main(int argc, char * argv[])
{
  std::string name, secondName, department;
  std::size_t age;

  custom_tie(name, secondName, age, department) = getPerson();

  assert(name == "Petia");
  assert(secondName == "Ivanoff");
  assert(age == 23);
  assert(department == "Sale");

  return 0;
}
