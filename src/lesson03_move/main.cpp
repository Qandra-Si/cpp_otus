#include <iostream>
#include <string>

void example1();
void example2();
void example3();
void example4();
void example5();
void example6();
void example7();

int main()
{
  example1();
  example2();
  example3();
  example4();
  example5();
  example6();
  example7();
  return 0;
}

void example1()
{
  std::string name{"Vasia Pupkin"};
  std::move(name);
  std::cout << name <<std::endl; // Vasia Pupkin
}

void example2()
{
  std::string name{"Petia Sidorov"};
  std::string name2 = std::move(name);
  std::cout << name << std::endl; // UB
}

void example3()
{
  std::string name{"Petia"};
  std::string name2 = std::move(name);
  std::cout << name << std::endl; // UB, но если работа с короткими строками оптимизирована, м.б. Petia
}

void example4()
{
  const std::string & ref = "Chuk"; // const reference to lvalue assigned rvalue
  std::cout << ref << std::endl;
}

std::string foo(std::string && str) noexcept
{
  std::string res;
  res.swap(str);
  return res;
}

void example5()
{
  std::string a{"Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."};
  std::cout << "\nHere a='" << a << "'" << std::endl;
  std::string b = foo(std::move(a));
  std::cout << "Moved a='" << a << "' into b='" << b << "'" << std::endl;
}

//---

struct X
{
  X(int) { std::cout << "X(int);\n"; }
  X(const X&, int = 1) { std::cout << "X(const X&, int);\n"; }
};

void example6()
{
  std::cout << "\nX::X(const X&) and X::X(X&,int=1) are copy constructors" << std::endl;
  X a(1);             // calls X(int);
  X b(a, 0);          // calls X(const X&, int);
  X c = b;            // calls X(const X&, int);
  // см. https://eel.is/c++draft/class.copy.ctor
}

//---

struct Y
{
  Y(int) { std::cout << "Y(int); // default constructor\n"; }
  Y(const Y&) { std::cout << "Y(const Y&); // copy constructor\n"; }
  Y(Y&&) { std::cout << "Y(Y&&); // move constructor\n"; }
};

#pragma GCC push_options
#pragma GCC optimize ("no-elide-constructors") // пришлось отключить RVO, иначе надо как-то создать Y
Y no_rvo(int i)
{
  return Y(i);
}
#pragma GCC pop_options

void example7()
{
  std::cout << "\nY::Y(Y&&) is a move constructor" << std::endl;
  Y d(no_rvo(1));          // calls Y(Y&&)
  Y e = d;            // calls Y(const Y&)
  // см. https://eel.is/c++draft/class.copy.ctor
}
