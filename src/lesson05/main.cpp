#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <cassert>


void example1(); // unique_ptr
void example2(); // deleter
void example3(); // shared_ptr
void example4(); // weak_ptr
void example5(); // конфликт с make_shared и weak_ptr
void example6(); // перекрёстные ссылки
void example7(); // enable_shared_from_this

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

void pass_unique_ptr_function(std::unique_ptr<int> a, std::function<void(int)> b)
{
  b(*a.get());
}

void example1()
{
  std::cout << "example1:" << std::endl;

  // нет гарантии, что выделение памяти new int{10} закончится до того, как
  // будет вызван указатель на функцию, внутри которой произойдёт обращение к
  // умному указателю (которого ещё нет) => исключение, потеря памяти
  pass_unique_ptr_function(
    std::unique_ptr<int>(new int{10}),
    [](int x) { std::cout << x << std::endl; }
  );

  // следующий код решает эту проблему (сначала создаём умный указатель), но
  // код выглядит слишком громоздким
  std::unique_ptr<int> p(new int{33});
  pass_unique_ptr_function(
    std::move(p),
    [](int x) { std::cout << x << std::endl; }
  );

  // следующий код уже не выглядит таким громоздким
  pass_unique_ptr_function(
    std::make_unique<int>(42), // был добавлен в C++14
    [](int x) { std::cout << x << std::endl; }
  );
}

int * init() { std::cout << __PRETTY_FUNCTION__ << std::endl; return new int(1); }
void deinit(int * ptr) { std::cout << __PRETTY_FUNCTION__ << std::endl; delete ptr; }

struct deleter_t
{
  void operator()(int * ptr) {
    deinit(ptr);
  }
};

void example2()
{
  std::cout << "example2:" << std::endl;

  std::unique_ptr<int, deleter_t> ptr1{init()};

  auto deleter = [](int * ptr){ deinit(ptr); };
  std::unique_ptr<int, decltype(deleter)> ptr3{init(), deleter};

  // реализация с std::function неэффективна по кол-ву используемой памяти для размещения unique_ptr
  std::unique_ptr<int, std::function<void(int*)>> ptr2{init(), [](int *ptr){ deinit(ptr); }};
}

void example3()
{
  std::cout << "example3:" << std::endl;

  auto ptr1 = std::make_shared<int>(33);

  std::shared_ptr<int> ptr4;
  {
    std::shared_ptr<int> ptr2(init(), &deinit);
    {
      auto ptr3 = ptr2;
      ptr4 = ptr3;
    }
  }
  std::cout << "ptr4 still alive" << std::endl;
}

void example4()
{
  std::cout << "example4:" << std::endl;

  // weak_ptr создаётся только на базе shared_ptr, предназначен для получения
  // информации "живы ли данные"? при этом не влияет на время жизни данных

  // в контрольном блоке shared_ptr есть указатели слабые и сильные ссылок,
  // сильные влияют на время жизни данных, слабые влияют на время жизни
  // контрольного блока

  std::weak_ptr<int> ptr1;

  {
    auto ptr2 = std::shared_ptr<int>(init(), &deinit);
    ptr1 = ptr2;
    std::shared_ptr<int> x = ptr1.lock(); // возвращает shared_ptr или null
    std::cout << "weak_ptr lock=" << x << std::endl;
    assert(x);
  }

  auto x = ptr1.lock();
  std::cout << "weak_ptr lock=" << x << std::endl;
  assert(!x);
}

struct object_t
{
  int id;
  object_t(int id) : id(id)
  {
    std::cout << "object_t ctor(" << id << ")" << std::endl;
  }
  ~object_t()
  {
    std::cout << "object_t dtor(" << id << ")" << std::endl;
  }
};

void example5()
{
  std::cout << "example5:" << std::endl;

  auto obj1 = std::shared_ptr<object_t>(new object_t(1));
  auto obj2 = std::shared_ptr<object_t>(new object_t(2));
  std::weak_ptr<object_t> * weak1 = new std::weak_ptr<object_t>();
  *weak1 = obj1;
  // weak1 никогда не удаляется (где-то в программе отслеживает состояние obj1)
  // но obj2 удаляется

  auto obj3 = std::make_shared<object_t>(3);
  auto obj4 = std::make_shared<object_t>(4);
  std::weak_ptr<object_t> * weak3 = new std::weak_ptr<object_t>();
  *weak3 = obj3;
  std::cout << "weak3=" << weak3->lock() << std::endl;

  // утечка памяти: в случае make_shared (который создаёт единый кусок в памяти
  // с указателями и на блок данных и на контрольный блок) эта память отдельно
  // друг от друга удалиться не может

  // Внимание! когда у нас есть make_shared и weak_ptr, которые держат
  //           контрольные блоки, - это ошибка в проектировании
}

struct crosslink_t
{
  ~crosslink_t()
  {
    std::cout << "crosslink_t dtor" << std::endl;
  }
  std::shared_ptr<crosslink_t> link;
};

void example6()
{
  std::cout << "example6:" << std::endl;

  // перекрёстные ссылки - частный случай неправильного проектирования
  auto obj1 = std::make_shared<crosslink_t>();
  auto obj2 = std::make_shared<crosslink_t>();
  obj1->link = obj2;
  obj2->link = obj1;

  // чтобы решить проблему - надо использовать weak_ptr
}

struct problem_t
{
  int id;
  problem_t(int id) : id(id)
  {
    std::cout << "problem_t ctor(" << id << ")" << std::endl;
  }
  ~problem_t()
  {
    std::cout << "problem_t dtor(" << id << ")" << std::endl;
  }
  std::shared_ptr<problem_t> get() { return std::shared_ptr<problem_t>(this); }
};

struct better_t : std::enable_shared_from_this<better_t>
{
  int id;
  better_t(int id) : id(id)
  {
    std::cout << "better_t ctor(" << id << ")" << std::endl;
  }
  ~better_t()
  {
    std::cout << "better_t dtor(" << id << ")" << std::endl;
  }
  std::shared_ptr<better_t> get() { return shared_from_this(); }
};

void example7()
{
  std::cout << "example7:" << std::endl;

  {
    problem_t p(33);
    // std::weak_ptr<problem_t> sp = p.get(); // munmap_chunk(): invalid pointer
  }

  try
  {
    better_t c(42);
    std::weak_ptr<better_t> sc = c.get(); // так нельзя: terminate called after throwing an instance of 'std::bad_weak_ptr'
  }
  catch(const std::bad_weak_ptr& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  {
    auto c = std::shared_ptr<better_t>{new better_t(9)};
    std::weak_ptr<better_t> sc = c->get(); // так можно, shared_ptr на объект есть
  }

  // см. также правильный метод с shared_from_this и удалённым конструктором: https://stackoverflow.com/a/42302517
}

