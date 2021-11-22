// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

void foo(int) { std::cout << "int\n"; }

struct foo
{
  foo(int) { std::cout << "foo\n"; }
};

//---

struct kuk
{
  kuk(int) { std::cout << "kuk\n"; }
};

//---

void example3();
void example4();

int main()
{
  foo(0); // int
  foo({0}); // int
  //foo{0}; // error: expected ‘;’ before ‘{’ token

  kuk(0); // kuk
  kuk{0}; // kuk

  example3();
  example4();

  return 0;
}

struct lol
{
  lol(int) { std::cout << "lol\n"; }
};

void example3()
{
  lol(0); // lol
  lol{0}; // lol
}

struct kek
{
  void operator()(int) { std::cout << "kek\n"; }
};
kek lol;

void example4()
{
  lol(0); // kek
  //lol{0}; // error: expected ‘;’ before ‘{’ token
}

