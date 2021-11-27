// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/matrix.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/matrix.h>
#include <core/fake_heap_allocator.h>
#include <core/fake_heap_helpers.h>

#include <string>
#include <cstdint>


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

/******************************************************************************
  canonical_assignment_operator
*******************************************************************************/

TEST(test_matrix, canonical_assignment_operator)
{
  {
    core::matrix_t<int, 0> matrix;

    matrix[100][100] = 314;

    ASSERT_EQ(1, matrix.size());
    ASSERT_EQ(314, matrix[100][100]);
  }

  {
    core::matrix_t<int, 0> matrix;

    (matrix[100][100] = 314) = 0;

    ASSERT_EQ(0, matrix.size());
  }

  {
    core::matrix_t<int, 0> matrix;

    ((matrix[100][100] = 314) = 0) = 217;

    ASSERT_EQ(1, matrix.size());
    ASSERT_EQ(217, matrix[100][100]);
  }
}

/******************************************************************************
  matrix_allocator
*******************************************************************************/

TEST(test_matrix, matrix_allocator)
{
  using value_type = uint64_t;
  using alloc_t = core::fake_heap_allocator_t<core::matrix_item_t<value_type>>;
  using matrix_t = core::matrix_t<value_type, 0, alloc_t>;

  class debugger_t : public core::fake_heap_debugger_t
  {
  public:
    virtual std::basic_ostream<char>& get() override { return std::cout; }
  } debugger;

  // === > fake_heap.reset from fake:008AC558 to fake:008AC620
  matrix_t::allocator_type a(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 200, &debugger }));
  // === > fake_heap.malloc from fake:008AC558 to fake:008AC578, size 32
  // === > fake_heap.malloc from fake:008AC578 to fake:008AC580, size 8
  // === > fake_heap.malloc from fake:008AC580 to fake:008AC598, size 24
  matrix_t matrix(a);

  // память в куче на операцию добавления 100 default-элементов не тратится (ограничена аллокатором)
  for (int i = 0; i < 100; ++i)
  {
    matrix[i][i] = 0;
  }
  ASSERT_EQ(0, matrix.size());

  // === > fake_heap.malloc from fake:008AC598 to fake:008AC5B0, size 24
  matrix[1][1] = 1;

  ASSERT_EQ(1, matrix.size());

  // === > fake_mem.free fake:008AC598
  // === > fake_mem.free fake:008AC580
  // === > fake_mem.free fake:008AC578
  // === > fake_mem.free fake:008AC558
}
