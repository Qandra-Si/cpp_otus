#include <iostream>

void example1();
void example2();

int main()
{
  example1();
  example2();
  return 0;
}

//------------------------------------------------------------------------------

class A
{
  const char * desc;
public:
  A(const char * desc = nullptr) : desc(desc?desc:"") { std::cout << "A::A(" << this->desc << ");\n"; }
  A( const A& a) : desc(a.desc) { std::cout << "A::A( const A& a = " << desc << " );\n"; }
  ~A() { std::cout << "A::~A(" << desc << ");\n"; }
};

A rvo()
{
  return A();
}

A nrvo()
{
  A local_nrvo;
  return local_nrvo;
}

A rvo_ext(bool c)
{
  if (c)
    return A("true");
  return A("false");
}

A nrvo_ext(bool c)
{
  if (c)
  {
    A local_nrvo{"true"};
    return local_nrvo;
  }
  else
  {
    A local_nrvo{"false"};
    return local_nrvo;
  }
}

void example1()
{
  {
    std::cout << "If you see just constructor and destructor - RVO supported by you compiler\n";
    A a = rvo();
  }

  {
    std::cout << "If you see just constructor and destructor - NRVO supported by you compiler\n";
    A a = nrvo();
  }

  int condition;
  std::cout << "Type condition (0 or 1): ";
  std::cin >> condition;

  {
    {
      std::cout << "If you see just constructor and destructor - more complex RVO supported by you compiler\n";
      A a = rvo_ext(condition != 0);
    }
    {
      std::cout << "If you see just constructor and destructor - more complex RVO supported by you compiler\n";
      A a = rvo_ext(condition == 0);
    }
  }

  {
    {
      std::cout << "If you see just constructor and destructor - complex inline NRVO supported by you compiler\n";
      A a = nrvo_ext(true);
    }
    {
      std::cout << "If you see just constructor and destructor - more complex NRVO supported by you compiler\n";
      A a = nrvo_ext(condition != 0);
    }
    {
      std::cout << "If you see just constructor and destructor - more complex NRVO supported by you compiler\n";
      A a = nrvo_ext(condition == 0);
    }
  }
}

//------------------------------------------------------------------------------

struct B
{
  void * p;
  B() : p(this) { }
};

B rvo_elision()
{
  return B();
}

B nrvo_elision()
{
  B res;
  return res;
}


void example2()
{
  {
    B b;
    std::cout << "\nwell-formed class.copy.elision, b.p=" << b.p << " points to b=" << &b << std::endl;
  }

  {
    B b = rvo_elision();
    std::cout << "\nwell-formed class.copy.elision, b.p=" << b.p << " can point to b=" << &b << " or be dangling" << std::endl;
  }

  {
    B b = nrvo_elision();
    std::cout << "\nwell-formed class.copy.elision, b.p=" << b.p << " can point to b=" << &b << " or be dangling" << std::endl;
  }
}
