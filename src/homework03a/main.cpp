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

struct custom_tie
{
  std::string& a, &b, &d;
  std::size_t& c;
  custom_tie(std::string& a, std::string& b, std::size_t& c, std::string& d) : a(a), b(b), c(c), d(d) { }
  void operator=(const std::tuple<std::string, std::string, std::size_t, std::string> & src)
  {
    a = std::get<0>(src);
    b = std::get<1>(src);
    c = std::get<2>(src);
    d = std::get<3>(src);
  }
};

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
