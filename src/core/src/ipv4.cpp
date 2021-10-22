// -*- mode: c++; coding: utf-8 -*-
#include <sstream>
#include <algorithm>

#include <core/ipv4.h>


namespace core
{

bool split_ipv4(const std::string & s, ipv4_t & ipv4)
{
  uint8_t * v_cursor = ipv4.v_addr;
  const uint8_t * v_end = v_cursor + sizeof(ipv4.v_addr);
  std::istringstream iss(s);
  std::string snum;
  while(std::getline(iss, snum, '.'))
  {
    if (v_cursor == v_end) return false;
    *v_cursor++ = (uint8_t)std::stoul(snum);
  }
  return v_cursor == v_end;
}

typedef int (CPP_OTUS_CDECL * TYPEOF_ipv4_cmp_c)(void const*, void const*);
void sort_ipv4_pool(ip_pool_t& pool, TYPEOF_ipv4_cmp pred)
{
  std::qsort(pool.data(), pool.size(), sizeof(ip_pool_t::value_type), reinterpret_cast<TYPEOF_ipv4_cmp_c>(pred));
}

int big_endian_ipv4_cmp(const ipv4_t*x, const ipv4_t*y)
{
  if (x->numeric < y->numeric) return -1;
  if (x->numeric > y->numeric) return 1;
  return 0;
}

// C++23 будет поддерживать метод реверса байт в числе, а пока используем макрос по старинке
#define be32(n32) ((n32>>24)&0xff) |    \
                  ((n32<<8)&0xff0000) | \
                  ((n32>>8)&0xff00) |   \
                  ((n32<<24)&0xff000000)

int little_endian_ipv4_cmp(const ipv4_t*x, const ipv4_t*y)
{
  const uint32_t _x = be32(x->numeric);
  const uint32_t _y = be32(y->numeric);
  if (_x < _y) return -1;
  if (_x > _y) return 1;
  return 0;
}

}
