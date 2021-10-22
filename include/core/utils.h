// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <string>
#include <vector>


namespace core {

std::string get_build_ticker();

std::vector<std::string> split(const std::string& s, char delimiter);
void split_into(const std::string& s, char delimiter, std::vector<std::string> & into);

}
