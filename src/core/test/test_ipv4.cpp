// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/utils.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/utils.h>
#include <core/ipv4.h>


/******************************************************************************
   split_incorrect
*******************************************************************************/

TEST(test_ipv4, split_incorrect)
{
  core::ipv4_t ipv4;

  ASSERT_FALSE(core::split_ipv4("127.0.0", ipv4));
  ASSERT_FALSE(core::split_ipv4("127.0.0.0.1", ipv4));

  ASSERT_TRUE(core::split_ipv4("127. 0.0.1", ipv4));
}

/******************************************************************************
   split_localhost
*******************************************************************************/

TEST(test_ipv4, split_localhost)
{
  core::ipv4_t ipv4;

  ASSERT_TRUE(core::split_ipv4("127.0.0.1", ipv4));
  ASSERT_EQ(127, ipv4.v_addr[0]);
  ASSERT_EQ(0, ipv4.v_addr[1]);
  ASSERT_EQ(0, ipv4.v_addr[2]);
  ASSERT_EQ(1, ipv4.v_addr[3]);
}

/******************************************************************************
   sort_ipv4_pool
*******************************************************************************/

TEST(test_ipv4, sort_ipv4_pool)
{
  core::ipv4_t ipv4;
  core::ip_pool_t pool;

  ASSERT_TRUE(core::split_ipv4("1.2.3.4", ipv4));
  pool.push_back(ipv4);

  ASSERT_TRUE(core::split_ipv4("255.255.255.255", ipv4));
  pool.push_back(ipv4);

  ASSERT_TRUE(core::split_ipv4("0.0.0.0", ipv4));
  pool.push_back(ipv4);

  core::sort_ipv4_pool(
    pool,
    core::is_little_endian() ?
      core::little_endian_ipv4_cmp_reverse :
      core::big_endian_ipv4_cmp_reverse
  );

  ASSERT_EQ(3, pool.size());
  ASSERT_EQ(0xffffffff, pool[0].v_addr[0] << 24 | pool[0].v_addr[1] << 16 | pool[0].v_addr[2] << 8 | pool[0].v_addr[3]);
  ASSERT_EQ(0x01020304, pool[1].v_addr[0] << 24 | pool[1].v_addr[1] << 16 | pool[1].v_addr[2] << 8 | pool[1].v_addr[3]);
  ASSERT_EQ(0x00000000, pool[2].v_addr[0] << 24 | pool[2].v_addr[1] << 16 | pool[2].v_addr[2] << 8 | pool[2].v_addr[3]);

  core::sort_ipv4_pool(
    pool,
    core::is_little_endian() ?
      core::little_endian_ipv4_cmp :
      core::big_endian_ipv4_cmp
  );

  ASSERT_EQ(3, pool.size());
  ASSERT_EQ(0x00000000, pool[0].v_addr[0] << 24 | pool[0].v_addr[1] << 16 | pool[0].v_addr[2] << 8 | pool[0].v_addr[3]);
  ASSERT_EQ(0x01020304, pool[1].v_addr[0] << 24 | pool[1].v_addr[1] << 16 | pool[1].v_addr[2] << 8 | pool[1].v_addr[3]);
  ASSERT_EQ(0xffffffff, pool[2].v_addr[0] << 24 | pool[2].v_addr[1] << 16 | pool[2].v_addr[2] << 8 | pool[2].v_addr[3]);
}
