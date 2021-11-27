// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include <core/matrix.h>


int main()
{
  {
    const core::matrix_t<int, -1> matrix; std::cout << std::endl;
    std::cout << "matrix.size() == " << matrix.size() << std::endl;
    std::cout << "matrix[0][1] == " << matrix[0][1] << std::endl;
    //matrix[2][2] = 33; // невозможно присваивать значения переменной, которая объявлена как константа
  } std::cout << std::endl;

  {
    core::matrix_t<int, -1> matrix; std::cout << std::endl;
    std::cout << "matrix.size() == " << matrix.size() << std::endl;
    std::cout << "matrix[2][2] == " << matrix[2][2] << std::endl;
    matrix[2][2] = 33;
  } std::cout << std::endl;

  core::matrix_t<int, -1> matrix; std::cout << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;
  auto a = matrix[0][0]; std::cout << std::endl; // [0]
  std::cout << "matrix[0][0] == " << a << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;

  matrix[100][100] = 314;
  std::cout << "matrix[100][100] == " << matrix[100][100] << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;

  // выведется одна строка
  // 100100314
  for (auto c : matrix)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = c;
    std::cout << x << y << v << std::endl;
  }

  matrix[100][100] = -1;

  for (core::matrix_t<int, -1>::iterator itr = matrix.begin(), end = matrix.end(); itr != end; ++itr)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = *itr;
    std::cout << x << y << v << std::endl;
  }

  return 0;
}
