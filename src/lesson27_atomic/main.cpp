// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <cstdint>
#include <atomic>


struct a1_t
{
  uint8_t x;
};

struct a2_t
{
  uint8_t x;
  uint8_t y;
};

struct a3_t
{
  uint8_t x;
  uint8_t y;
  uint8_t z;
};

struct a4_t
{
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t w;
};

struct a5_t
{
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t w;
  uint8_t q;
};

struct a7_t
{
  uint8_t x1;
  uint8_t x2;
  uint8_t x3;
  uint8_t x4;
  uint8_t x5;
  uint8_t x6;
  uint8_t x7;
};

struct a8_t
{
  uint8_t x1;
  uint8_t x2;
  uint8_t x3;
  uint8_t x4;
  uint8_t x5;
  uint8_t x6;
  uint8_t x7;
  uint8_t x8;
};

struct a16_t
{
  uint8_t x1;
  uint8_t x2;
  uint8_t x3;
  uint8_t x4;
  uint8_t x5;
  uint8_t x6;
  uint8_t x7;
  uint8_t x8;
  uint8_t x9;
  uint8_t x10;
  uint8_t x11;
  uint8_t x12;
  uint8_t x13;
  uint8_t x14;
  uint8_t x15;
  uint8_t x16;
};

struct owo_t
{
  uint8_t  o1;
  uint16_t w;
  uint8_t  o2;
};

// будет lock_free
#pragma pack(push, 1)
struct powo_t
{
  uint8_t  o1;
  uint16_t w;
  uint8_t  o2;
};
#pragma pack(pop)

// будет lock_free
#pragma pack(push, 1)
struct powdo_t
{
  uint8_t  o1;
  uint16_t w;
  uint32_t d;
  uint8_t  o2;
};
#pragma pack(pop)
// то есть ситуация с uint8_t + uint16_t + uint8_t выше - касается alignment-а структуры в 5 октет (шаблон atomic за нас alignment делать не будет и не должен)

int main()
{
  std::cout << "a1_t: " << (int)std::atomic<a1_t>{}.is_lock_free() << std::endl;
  std::cout << "a2_t: " << (int)std::atomic<a2_t>{}.is_lock_free() << std::endl;
  std::cout << "a3_t: " << (int)std::atomic<a3_t>{}.is_lock_free() << std::endl;
  std::cout << "a4_t: " << (int)std::atomic<a4_t>{}.is_lock_free() << std::endl;
  std::cout << "a5_t: " << (int)std::atomic<a5_t>{}.is_lock_free() << std::endl;
  std::cout << "a7_t: " << (int)std::atomic<a7_t>{}.is_lock_free() << std::endl;
  std::cout << "a8_t: " << (int)std::atomic<a8_t>{}.is_lock_free() << std::endl;
  std::cout << "a16_t: " << (int)std::atomic<a16_t>{}.is_lock_free() << std::endl;
  std::cout << "owo_t: " << (int)std::atomic<owo_t>{}.is_lock_free() << std::endl;
  std::cout << "powo_t: " << (int)std::atomic<powo_t>{}.is_lock_free() << std::endl;
  std::cout << "powdo_t: " << (int)std::atomic<powdo_t>{}.is_lock_free() << std::endl;
  return 0;
}

