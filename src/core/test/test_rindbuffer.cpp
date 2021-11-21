// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/ringbuffer.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/ringbuffer.h>

#include <string>
#include <vector>


/******************************************************************************
  basic
*******************************************************************************/

TEST(test_ringbuffer, basic)
{
  core::ringbuffer_t<int, 10> rb1;
  decltype(rb1) rb2;

  ASSERT_FALSE(std::is_trivial<decltype(rb1)>::value);
  ASSERT_FALSE(std::is_trivially_copyable<decltype(rb1)>::value);
  ASSERT_FALSE(std::is_swappable<decltype(rb1)>::value);

  bool a = std::is_assignable<decltype(rb1), decltype(rb2)>::value;
  ASSERT_FALSE(a);

  ASSERT_EQ(10, rb1.max_size());
  ASSERT_EQ(10, rb2.max_size());

  ASSERT_NO_THROW(rb1.push(33));

  ASSERT_FALSE(rb1.empty());
  ASSERT_TRUE(rb2.empty());

  //ASSERT_EQ(1, rb1.size());
  //ASSERT_EQ(0, rb2.size());

  int val;
  ASSERT_TRUE(rb1.pop(val));

  ASSERT_TRUE(rb1.empty());
  ASSERT_TRUE(rb2.empty());
}

/******************************************************************************
  rlvalue
  см. https://en.cppreference.com/w/cpp/language/template_argument_deduction
*******************************************************************************/

TEST(test_ringbuffer, rlvalue)
{
  core::ringbuffer_t<int, 10> rb;

  int i = 10;
  const int ci = 15;
  const int& cir = ci;
  int* pi = &i;
  const int* cpi = &i;

  ASSERT_TRUE(rb.push(i));   // foo<int&>(int&) - специальный случай
  ASSERT_TRUE(rb.push(ci));  // foo<int&>(const int&) - аналогично, это т.н. forwarding-reference 
  ASSERT_TRUE(rb.push(cir)); // reference удаляется, остаётся foo<int&>(const int&)
  ASSERT_TRUE(rb.push(42));  // параметр не lvalue, foo<int>(int&&)
  ASSERT_TRUE(rb.push(pi));  // foo<int&>(int*&)
  ASSERT_TRUE(rb.push(cpi)); // foo<int&>(const int*&)

  EXPECT_THROW(rb.push(nullptr), std::invalid_argument);

  int val;

  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(10, val);
  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(15, val);
  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(15, val);
  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(42, val);
  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(10, val);
  ASSERT_TRUE(rb.pop(val));
  ASSERT_EQ(10, val);
  ASSERT_FALSE(rb.pop(val));
}

/******************************************************************************
  string
*******************************************************************************/

TEST(test_ringbuffer, string)
{
  core::ringbuffer_t<std::string, 3> rb;
  std::string val;

  ASSERT_TRUE(rb.push("— Скажи-ка, дядя, ведь не даром"));
  ASSERT_TRUE(rb.push("Москва, спаленная пожаром,"));
  ASSERT_FALSE(rb.push("Французу отдана?"));

  ASSERT_TRUE(rb.pop(val));
  ASSERT_STREQ("— Скажи-ка, дядя, ведь не даром", val.c_str());

  ASSERT_TRUE(rb.push("Французу отдана?"));

  ASSERT_TRUE(rb.pop(val));
  ASSERT_STREQ("Москва, спаленная пожаром,", val.c_str());
  ASSERT_TRUE(rb.pop(val));
  ASSERT_STREQ("Французу отдана?", val.c_str());
}

/******************************************************************************
  iterator_basic
*******************************************************************************/

TEST(test_ringbuffer, iterator_basic)
{
  using ring_t = core::ringbuffer_t<int, 10>;

  ring_t rb;

  ASSERT_EQ(rb.begin(), rb.begin());
  ASSERT_EQ(rb.end(), rb.begin());
  ASSERT_EQ(rb.end(), rb.end());

  //ASSERT_EQ(rb.begin(), rb.cbegin());
  //ASSERT_EQ(rb.end(), rb.cend());
  //ASSERT_EQ(rb.cend(), rb.begin());
  //ASSERT_EQ(rb.end(), rb.cbegin());
  //ASSERT_EQ(rb.cbegin(), rb.cbegin());

  for (const auto& ref : rb)
  {
    ASSERT_TRUE(false); // контейнер пуст, сюда мы попасть не должны
  }

  ring_t::iterator end = rb.end();
  ring_t::iterator begin = rb.begin();
  ASSERT_EQ(end, begin);
  ASSERT_EQ(end, rb.begin());
  ASSERT_EQ(begin, rb.end());

  rb.push(33);

  bool found = false;
  for (const auto& ref : rb)
  {
    found = true; // контейнер не пуст
    break;
  }
  ASSERT_TRUE(found);
}

/******************************************************************************
  non_compatible_iterators
*******************************************************************************/

TEST(test_ringbuffer, non_compatible_iterators)
{
  using ring_t = core::ringbuffer_t<int, 10>;

  ring_t rb1, rb2;

  EXPECT_DEBUG_DEATH(rb1.begin() == rb2.begin(), "");
  EXPECT_DEBUG_DEATH(rb1.end() == rb2.end(), "");
}

/******************************************************************************
  iterator_move_forward
*******************************************************************************/

TEST(test_ringbuffer, iterator_move_forward)
{
  using ring_t = core::ringbuffer_t<int, 10>;

  ring_t rb;

  rb.push(3);
  rb.push(15);
  rb.push(10);
  rb.push(20);

  ring_t::iterator end = rb.end();
  ring_t::iterator itr = rb.begin();

  ASSERT_NE(end, itr);

  end = rb.end();
  itr = rb.begin();

  ASSERT_NE(end, itr);

  ASSERT_EQ(3, *(++itr));
  ASSERT_EQ(3, *(itr++));
  ASSERT_EQ(10, *(++itr));
  ASSERT_EQ(10, *(itr++));
  //ASSERT_EQ(20, *itr);
  ASSERT_EQ(end, ++itr);

  for (ring_t::iterator itr = rb.begin(), end = rb.end(); itr != end; ++itr)
  {
    ASSERT_TRUE(false); // контейнер пуст, сюда мы попасть не должны
  }

  rb.push(9);
  rb.push(5);
  rb.push(4);
  rb.push(7);

  std::vector<int> v{9, 5, 4, 7};
  for (ring_t::iterator itr = rb.begin(), end = rb.end(); itr != end; ++itr)
  {
    const auto& ref = *itr;
    ASSERT_FALSE(v.empty());
    ASSERT_EQ(v.front(), ref);
    v.erase(v.begin());
  }
  ASSERT_TRUE(v.empty());
}

/******************************************************************************
  iterator_end
*******************************************************************************/

TEST(test_ringbuffer, iterator_end)
{
  using ring_t = core::ringbuffer_t<int, 5>;

  ring_t rb;

  rb.push(3);
  rb.push(15);
  rb.push(10);
  rb.push(20);

  std::vector<int> v{ 3, 15, 10, 20 };
  for (ring_t::iterator itr = rb.begin(), end = rb.end(); itr != end; ++itr)
  {
    const auto& ref = *itr;
    std::cout << ref << std::endl;
    ASSERT_FALSE(v.empty());
    ASSERT_EQ(v.front(), ref);
    v.erase(v.begin());
  }
  ASSERT_TRUE(v.empty());
}
