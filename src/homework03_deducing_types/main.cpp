// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <utility>


template <typename T>
void func1(T param) {
  (void)(param);
}

template <typename T>
void func2(T& param) {
  (void)(param);
}

template <typename T>
void func3(T* param) {
  (void)(param);
}

template <typename T>
void func4(T&& param) {
  (void)(param);
}

void responses1()
{
  int i = 10; 
  const int ci = 15; 
  const int &cir = ci; 
  int * pi = &i; 
  const int * cpi = &i; 
 
  func1(i); // 1.1. int
  func1(ci); // 1.2. int - const удаляется 
  func1(cir); // 1.3. int - const удаляется, reference удаляется
  func1(42); // 1.4. int 
  func1(pi); // 1.5. int * 
  func1(cpi); // 1.6. const int *
  //func1({1}); // 1.7. <--- не компилируется, нам нужен void func1(std::initializer_list<T>)
  //func1({1, 2, 3}); // 1.8. <--- аналогично [1.7]

  func2(i); // 2.1. int& - т.к. reference явно прописан в списке параметров
  func2(ci); // 2.2. const int& - const НЕ удаляется, reference явно прописан
  func2(cir); // 2.3. const int& - const НЕ удаляется, reference удаляется (аналогично [1.3]), далее явно прописывается reference
  //func2(42); // 2.4. <--- не компилируется, т.к. reference на литерал не м.б. создан 
  func2(pi); // 2.5. int *& - reference явно прописан в списке параметров
  func2(cpi); // 2.6. const int*& - const НЕ удаляется, reference парописан явно

  //func3(i); // 3.1. <--- не компилируется, i не является указателем и не имеет совместимого преобразования к указателю 
  //func3(ci); // 3.2. <--- аналогично сказанному в [3.1]
  //func3(cir); // 3.3. <--- аналогично сказанному в [3.1] 
  //func3(42); // 3.4. <-- аналогично сказанному [3.1]
  func3(pi); // 3.5. int* - pi является указателем
  func3(cpi); // 3.6. const int* - const НЕ удаляется, cpi является указателем

  // так... пришлось залезть в https://en.cppreference.com/w/cpp/language/template_argument_deduction
  
  func4(i); // 4.1. func4<int&>(int&) - специальный случай
  func4(ci); // 4.2. func4<int&>(const int&) - аналогично, это т.н. forwarding-reference 
  func4(cir); // 4.3. сложнааааа... reference удаляется (вроде), остаётся func4<int&>(const int&)
  func4(42); // 4.4. парамтер не lvalue, func4<int>(int&&)
  func4(pi); // 4.5. func4<int&>(int*&)
  func4(cpi); // 4.6. func4<int&>(const int*&)
}

int func_value() {
  return 42;
}

int& func_ref() {
  static int value = 42;
  return value;
}

const int& func_cref() {
  static const int value = 42;
  return value;
}

int* func_ptr() {
  static int value = 42;
  return &value;
} 

const int * func_cptr() {
  static int value = 42;
  return &value;
}

const int * const func_ccptr() {
  static const int value = 42;
  return &value;
}

// Don't do this at home!!!
int&& func_rref() {
  int value = 42;
  // o_O
  // после return-а стек будет разрушен, вместе с value и ссылка станет невалидной
  // а поскольку компилятор "подумает", что программист не способен на такую ошибку,
  // то он вполне может сгенерировать код, который не будет валидным не только после
  // вызова метода, но и ;) ДО вызова метода
  return std::move(value);
}

// подробнее по auto&& см. тут https://stackoverflow.com/a/13242177
void responses2()
{
  { 
    auto v1 = func_value(); // 1.1 - int
    //auto& v2 = func_value(); // 1.2 <-- ошибка компиляции, int не может быть приведён к int& (мы же не пишем int & var = 42)
    const auto& v3 = func_value();  // 1.3 - const int& (разрешено)
    auto&& v4 = func_value(); // 1.4 - int&&
  }
  {
    auto v1 = func_ref(); // 2.1 - int, reference удаляется
    auto& v2 = func_ref(); // 2.2 - int&, reference удаляется, потом явно прописывается
    const auto& v3 = func_ref(); // 2.3 - const int&, reference удаляется, потом const reference явно прописываются
    auto&& v4 = func_ref(); // 2.4 - int&, reference удаляется, потом явно прописывается
  }
  {
    auto v1 = func_cref(); // 3.1 - int, const reference удаляются
    auto& v2 = func_cref(); // 3.2 - const int&, const reference НЕ удаляются
    const auto& v3 = func_cref(); // 3.3 - const int&, const reference НЕ удаляются
    auto&& v4 = func_cref(); // 3.4 - const int&, const reference НЕ удаляются
  }
  {
    auto v1 = func_ptr(); // 4.1 - int*
    //auto& v2 = func_ptr(); // 4.2 <--- ошибка компиляции, аналогично пункту [1.2]
    const auto& v3 = func_ptr(); // 4.3 - int* const&, const относится к reference
    auto&& v4 = func_ptr(); // 4.4 - int*&&
  }
  {
    auto v1 = func_cptr(); // 5.1 - const int*, у pointer const не удаляется
    //auto& v2 = func_cptr(); // 5.2 <--- ошибка компиляции, аналогично пункту [1.2]
    const auto& v3 = func_cptr(); // 5.3 - const int * const &, const относится к reference, const pointer сохранён
    //auto&& v4 = func_cptr(); // 5.4 - const int*&&
  }
  {
    auto v1 = func_ccptr(); // 6.1 - const int *, const относящийся к значению удаляется, остаётся только const pointer-а
    //auto& v2 = func_ccptr(); // 6.2 <--- ошибка компиляции, аналогично пункту [1.2]
    const auto& v3 = func_ccptr(); // 6.3 - const int * const &, const относится к reference, const pointer сохранён
    auto&& v4 = func_ccptr(); // 6.4 - const int*&&
  }
  {
    auto v1 = func_rref(); // 7.1 - int
    //auto& v2 = func_rref(); // 7.2 <--- ошибка компиляции, мы не можем написать int & v2 = 42;
    const auto& v3 = func_rref(); // 7.3 - const int &, а тут разрешено
    auto&& v4 = func_rref(); // 7.4 - int &&
  }
  {
    auto v1(10); // 8.1 - int
    auto v2 = 10; // 8.2 - аналогично
    auto v3{10}; // 8.3 - int
    //auto v4 = {10}; // 8.4 <--- не выводится
    //auto v5 = {1, 2, 3}; // 8.5 <--- аналогично
    //auto v6 = {1, 2, 3.0}; // 8.6 <--- аналогично
  }
}

int main(int argc, char* argv[])
{
  responses1();
  responses2();

  return EXIT_SUCCESS;
}

