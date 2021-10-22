// -*- mode: c++; coding: utf-8 -*-
#include <sstream>

#include <core/utils.h>


namespace core
{

std::string get_build_ticker()
{
  return __DATE__;
}

std::vector<std::string> split(const std::string & s, char delimiter)
{
  std::vector<std::string> items;
  split_into(s, delimiter, items);
  return items;
}

void split_into(const std::string& s, char delimiter, std::vector<std::string>& into)
{
  into.clear();
  std::string item;
  std::istringstream iss(s);
  while (std::getline(iss, item, delimiter))
  {
    into.push_back(item);
  }
}

}
