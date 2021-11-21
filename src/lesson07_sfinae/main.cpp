// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <vector>
#include <set>
#include <string>


void example1(); // sfinae concept
void example2(); // enable_if

int main()
{
  example1();
  example2();
  return 0;
}

struct A {
  void find();
};
struct A1 : A {
  void bar();
};
struct B {
  void finding();
};
struct C {
  int find;
};

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

void example1()
{
  std::cout << "vector: " << DetectFind<std::vector<int> >::value << ", "
            << "set: " << DetectFind<std::set<int> >::value << ", "
            << "A: " << DetectFind<A>::value << ", "
            << "A1: " << DetectFind<A1>::value << ", "
            << "B: " << DetectFind<B>::value << ", "
            << "C: " << DetectFind<C>::value << std::endl;
}

struct some_struct_t
{
  some_struct_t(const std::string & str) : m_value(str) { }
  some_struct_t(const char * cstr) : m_value(cstr) { } // легче
  some_struct_t(const std::string && str) : m_value(std::move(str)) { }

  // уменьшаем комбинаторную сложность кол-ва конструкторов, если m_value не один
  template <typename T>
  some_struct_t(T&& str) {
    m_value = std::forward<T>(str);
  }
  some_struct_t(const some_struct_t & other) : m_value{other.m_value}
  {
  }

  template <typename T, typename C>
  some_struct_t(T&& str, C&& str2) {
    m_value = std::forward<T>(str);
    m_value2 = std::forward<C>(str2);
  }
private:
  std::string m_value;
  std::string m_value2;
  std::string m_value3;
};

struct some_struct2_t
{
  template<typename T,
           typename Fake = typename std::enable_if<
             !std::is_same<typename std::remove_reference<T>::type,
                           some_struct2_t>::value,
             void>::type
           >
  some_struct2_t(T&& str)
  {
    m_value = std::forward<T>(str);
  }

  some_struct2_t(const some_struct2_t & other) : m_value{other.m_value}
  {
  }
private:
  std::string m_value;
  std::string m_value2;
  std::string m_value3;
};

void example2()
{
  some_struct_t first("Hello, World!");
  //some_struct_t second = first; // так нельзя, т.к. компилятор предпочтёт template конструктору у которого указан const (которого нет у first)

  const some_struct_t third("foo");
  some_struct_t fourth = third; // а так можно, т.к. конструктор идеально совпал

  some_struct2_t first2("Hello, Wordl!");
  some_struct2_t second2 = first2; // а так проблема решена (компилятор не выдаёт ошибку и продолжает искать другие варианты для инстанциирования)
}
