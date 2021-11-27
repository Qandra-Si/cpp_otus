// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include <core/matrix.h>


/*! \brief main
 *
 * Ожидается вывод на экран следующих результатов:
 *
\code

1 0 0 0 0 0 0 8
0 2 0 0 0 0 7 0
0 0 3 0 0 6 0 0
0 0 0 4 5 0 0 0
0 0 0 4 5 0 0 0
0 0 3 0 0 6 0 0
0 2 0 0 0 0 7 0
1 0 0 0 0 0 0 8
18
matrix[1][1] = 1
matrix[2][2] = 2
matrix[3][3] = 3
matrix[4][4] = 4
matrix[5][5] = 5
matrix[6][6] = 6
matrix[7][7] = 7
matrix[8][8] = 8
matrix[9][9] = 9
matrix[0][9] = 9
matrix[1][8] = 8
matrix[2][7] = 7
matrix[3][6] = 6
matrix[4][5] = 5
matrix[5][4] = 4
matrix[6][3] = 3
matrix[7][2] = 2
matrix[8][1] = 1
matrix[100][100] = 0, size = 0
matrix[100][100] = 314, size = 1
matrix[100][100] = 0, size = 0
matrix[100][100] = 217, size = 1
\endcode
*/
int main()
{
  // При запуске программы необходимо создать матрицу с пустым значением 0,...
  core::matrix_t<int, 0> matrix;
  // ..., заполнить главную диагональ матрицы(от[0, 0] до[9, 9]) значениями от 0 до 9.
  for (int i = 0; i <= 9; ++i)
  {
    matrix[i][i] = i;
  }
  // Второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0.
  for (int i = 0; i <= 9; ++i)
  {
    matrix[i][9-i] = 9-i;
  }
  // Необходимо вывести фрагмент матрицы от[1, 1] до[8, 8]. Между столбцами пробел.
  // Каждая строка матрицы на новой строке консоли.
  for (int i = 1; i <= 8; ++i)
  {
    for (int j = 1; j <= 8; ++j)
    {
      if (j > 1) std::cout << ' ';
      std::cout << matrix[i][j];
    }
    std::cout << std::endl;
  }
  // Вывести количество занятых ячеек.
  std::cout << matrix.size() << std::endl;
  // Вывести все занятые ячейки вместе со своими позициями.
  for (auto c : matrix)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = c;
    std::cout << "matrix[" << x << "][" << y << "] = " << v << std::endl;
  }

  // Опционально реализовать N-мерную матрицу
  // (попробовал реализовать методом циклического метапрограммирования... не справился с передачей адреса)

  // Опционально реализовать каноническую форму оператора '=', допускающую выражения
  // ((matrix[100][100] = 314) = 0) = 217
  {
    core::matrix_t<int, 0> matrix;

    std::cout << "matrix[100][100] = " << matrix[100][100] << ", size = " << matrix.size() << std::endl;
    std::cout << "matrix[100][100] = " << (matrix[100][100] = 314) << ", size = " << matrix.size() << std::endl;
    std::cout << "matrix[100][100] = " << ((matrix[100][100] = 314) = 0) << ", size = " << matrix.size() << std::endl;
    std::cout << "matrix[100][100] = " << (((matrix[100][100] = 314) = 0) = 217) << ", size = " << matrix.size() << std::endl;
  }

  // также см. тесты в файле src/core/test/test_matrix.cpp

  return 0;
}
