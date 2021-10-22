// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/utils.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/utils.h>


/******************************************************************************
   split_nothing
*******************************************************************************/

TEST(test_utils, split_nothing)
{
  std::vector<std::string> lexems;

  lexems = core::split("", '.');
  ASSERT_TRUE(lexems.empty());

  lexems = core::split("    ", '?');
  ASSERT_EQ(1, lexems.size());

  lexems = core::split(" \t ", '\t');
  ASSERT_EQ(2, lexems.size());
}

/******************************************************************************
   split_by_tab
*******************************************************************************/

TEST(test_utils, split_by_tab)
{
  std::string lexems_line = "first\tsecond\tthird";

  std::vector<std::string> lexems = core::split(lexems_line, '\t');

  ASSERT_EQ(3, lexems.size());
  ASSERT_STREQ("first", lexems[0].c_str());
  ASSERT_STREQ("second", lexems[1].c_str());
  ASSERT_STREQ("third", lexems[2].c_str());
}

/******************************************************************************
   split_by_space
*******************************************************************************/

TEST(test_utils, split_by_space)
{
  std::string lexems_line = "1 2 3 4";

  std::vector<std::string> lexems = core::split(lexems_line, ' ');

  ASSERT_EQ(4, lexems.size());
  ASSERT_STREQ("1", lexems[0].c_str());
  ASSERT_STREQ("2", lexems[1].c_str());
  ASSERT_STREQ("3", lexems[2].c_str());
  ASSERT_STREQ("4", lexems[3].c_str());
}

/******************************************************************************
   split_by_dot
*******************************************************************************/

TEST(test_utils, split_by_dot)
{
  std::string lexems_line = "127.0.0.1";

  std::vector<std::string> lexems = core::split(lexems_line, '.');

  ASSERT_EQ(4, lexems.size());
  ASSERT_STREQ("127", lexems[0].c_str());
  ASSERT_STREQ("0", lexems[1].c_str());
  ASSERT_STREQ("0", lexems[2].c_str());
  ASSERT_STREQ("1", lexems[3].c_str());
}
