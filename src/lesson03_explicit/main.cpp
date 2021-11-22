// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

struct my_t { explicit my_t(int, int) {} };

struct other_t { other_t(int, int) {} };

//void foo(int) { std::cout << "int\n"; }
void foo(struct my_t) { std::cout << "my\n"; }
void foo(struct other_t) { std::cout << "other\n"; }

int main()
{
  my_t a{1, 2};
  foo({3, 4}); // gcc fail, clang ok
}
