// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <cstdint>
#include <string>
#include <vector>


namespace core {

union ipv4_t
{
  uint8_t v_addr[4];
  uint32_t numeric;
};

typedef std::vector<ipv4_t> ip_pool_t;

bool split_ipv4(const std::string& s, ipv4_t& ipv4);

typedef int(__cdecl* TYPEOF_ipv4_cmp)(const ipv4_t*, const ipv4_t*);
void sort_ipv4_pool(ip_pool_t& pool, TYPEOF_ipv4_cmp pred);

int big_endian_ipv4_cmp(const ipv4_t* x, const ipv4_t* y);
int little_endian_ipv4_cmp(const ipv4_t* x, const ipv4_t* y);

}
