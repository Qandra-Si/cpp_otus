// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/matrix.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/matrix.h>

#include <string>


/******************************************************************************
  basic
*******************************************************************************/

TEST(test_matrix, basic)
{
  core::matrix_t<int, -1> matrix;

  ASSERT_EQ(0, matrix.size());

  auto a = matrix[0][0];

  ASSERT_EQ(-1, a);
  ASSERT_EQ(0, matrix.size());

  matrix[100][100] = 314;

  ASSERT_EQ(314, matrix[100][100]);
  ASSERT_EQ(1, matrix.size());

  // выведется одна строка
  // 100100314
  std::string res;
  for (auto c : matrix)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = c;
    res += std::to_string(x) + std::to_string(y) + std::to_string(v);
  }
  ASSERT_STREQ("100100314", res.c_str());
}

/******************************************************************************
  remove
*******************************************************************************/

TEST(test_matrix, remove)
{
  core::matrix_t<char, ' '> matrix;

  matrix[0xffffffff][0xffffffff] = 'A';

  ASSERT_EQ('A', matrix[0xffffffff][0xffffffff]);
  ASSERT_EQ(1, matrix.size());

  matrix[0xffffffff][0xffffffff] = ' ';

  ASSERT_EQ(0, matrix.size());
}

/******************************************************************************
  default_value
*******************************************************************************/

TEST(test_matrix, default_value)
{
  core::matrix_t<int, 0> matrix;

  matrix[0][0] = 0;

  ASSERT_EQ(0, matrix.size());

  matrix[1][1] = 1;

  ASSERT_EQ(1, matrix.size());
}
