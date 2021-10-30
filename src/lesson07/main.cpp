// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <vector>
#include <set>


template<typename T>
class DetectFind
{
    struct Fallback { int find; }; // add member name "find"
    struct Derived : T, Fallback { };

    template<typename U, U> struct Check;

    typedef char Yes[1];  // typedef for an array of size one.
    typedef char No[2];  // typedef for an array of size two.

    template<typename U>
    static No& func(Check<int Fallback::*, &U::find> *);

    template<typename U>
    static Yes& func(...);

  public:
    typedef DetectFind type;
    enum { value = sizeof(func<Derived>(0)) == sizeof(Yes) };
};

int main()
{
    std::cout << DetectFind<std::vector<int> >::value << ' '
              << DetectFind<std::set<int> >::value << std::endl;
    return 0;
}
