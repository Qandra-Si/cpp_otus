#include <gtest/gtest.h>

#include <cpp_otus_version.h>


TEST(gtest_homework01, check_version)
{
  ASSERT_STREQ("1.4.3", PROJECT_VERSION);
}
