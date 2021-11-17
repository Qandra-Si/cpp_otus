// -*- mode: c++; coding: utf-8 -*-
#include <sstream>
#include <algorithm>

#include <core/ipv4.h>


namespace core
{

bool split_ipv4(const std::string & s, ipv4_t & ipv4)
{
  unsigned long num, idx = 0;
  std::istringstream iss(s);
  std::string snum;
  while(std::getline(iss, snum, '.'))
  {
    if (idx == 4) return false;
    try
    {
      num = std::stoul(snum);
    }
    catch (const std::invalid_argument&)
    {
      return false; // на вход пришло "a.b.c.d"
    }
    catch (const std::out_of_range&)
    {
        return false; // на вход пришло "4294967296.0.0.0"
    }
    if (num > 255) return false;
    ipv4.at(idx++) = (uint8_t)num;
  }
  return idx == 4;
}

typedef int (CPP_OTUS_CDECL * TYPEOF_ipv4_cmp_c)(void const*, void const*);
void sort_ipv4_pool(ip_pool_t& pool, TYPEOF_ipv4_cmp pred)
{
  std::qsort(pool.data(), pool.size(), sizeof(ip_pool_t::value_type), reinterpret_cast<TYPEOF_ipv4_cmp_c>(pred));
}

int big_endian_ipv4_cmp(const ipv4_t*x, const ipv4_t*y)
{
  const uint32_t x_numeric = *reinterpret_cast<const uint32_t*>(x->data());
  const uint32_t y_numeric = *reinterpret_cast<const uint32_t*>(y->data());
  if (x_numeric < y_numeric) return -1;
  if (x_numeric > y_numeric) return 1;
  return 0;
}

// C++23 будет поддерживать метод реверса байт в числе, а пока используем макрос по старинке
#define be32(n32) ((n32>>24)&0xff) |    \
                  ((n32<<8)&0xff0000) | \
                  ((n32>>8)&0xff00) |   \
                  ((n32<<24)&0xff000000)

int little_endian_ipv4_cmp(const ipv4_t*x, const ipv4_t*y)
{
  uint32_t x_numeric = *reinterpret_cast<const uint32_t*>(x->data());
  uint32_t y_numeric = *reinterpret_cast<const uint32_t*>(y->data());
  x_numeric = be32(x_numeric);
  y_numeric = be32(y_numeric);
  if (x_numeric < y_numeric) return -1;
  if (x_numeric > y_numeric) return 1;
  return 0;
}

}
