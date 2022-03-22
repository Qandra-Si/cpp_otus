// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <cstdint>
#include <cstring>


#if 0
  using value_type = uint8_t;
  #define value_type_divisor 10
#else
  using value_type = uint32_t;
  #define value_type_divisor 1000000000ull
#endif

unsigned multiply(int x, value_type res[], unsigned res_size)
{
  uint64_t prod, carry = 0;
  for (int i = 0; i < res_size; i++)
  {
    prod = (uint64_t)res[i] * x + carry;
    res[i] = prod % value_type_divisor;
    carry  = prod / value_type_divisor;
  }
  while (carry)
  {
    res[res_size] = carry % value_type_divisor;
    carry = carry / value_type_divisor;
    res_size++;
  }
  return res_size;
}

unsigned fact(int n, value_type *digits, unsigned digits_sz)
{
  digits[0] = 1;
  unsigned digits_qty = 1;
  for (int x = 2; x <= n; ++x)
  {
    digits_qty = multiply(x, digits, digits_qty);
  }
  //debug:std::cout << n << "! = "; for (int i = digits_qty-1; i >= 0; i--) std::cout << (uint32_t)digits[i] << " "; std::cout << std::endl;
  return digits_qty;
}

unsigned modulo(value_type *digits, unsigned digits_qty, unsigned divisor)
{
  uint64_t res = 0;
  for (int i = digits_qty-1; i >= 0; i--)
  {
    res = (res * value_type_divisor + digits[i]) % divisor;
  }
  return res;
}

/*! \brief Задача №1528. Sequence
*
* https://acm.timus.ru/problem.aspx?space=1&num=1528
*
* You are given a recurrent formula for a sequence f:
*  f(n) = 1 + f(1)g(1) + f(2)g(2) + … + f(n−1)g(n−1),
* where g is also a recurrent sequence given by formula:
*  g(n) = 1 + 2g(1) + 2g(2) + 2g(3) + … + 2g(n−1) − g(n−1)g(n−1).
* It is known that f(1) = 1, g(1) = 1. Your task is to find f(n) mod p.
*
* Исходные данные: The input consists of several cases. Each case contains two
* numbers on a single line. These numbers are n (1 <= n <= 10000) and
* p (2 <= p <= 2*10^9). The input is terminated by the case with n = p = 0 which
* should not be processed. The number of cases in the input does not exceed 5000.
*
* Результат: Output for each case the answer to the task on a separate line.
*/
int main()
{
  int n, p;

  // эта задача не про работу с большими числами (закомментировал, но оставил на память)
  // достаточно на каждой итерации считать модуль числа для формирования результата
  while (std::cin >> n >> p)
  {
    if (!n && !p) break;
    uint64_t res = 1;
    for (int x = 2; x <= n; ++x)
      res = (res * x) % p;
    std::cout << res << std::endl;
  }

#if 0
  value_type digits[35660/sizeof(value_type)];
  memset(digits, 0, sizeof(digits));

  while (std::cin >> n >> p)
  {
    if (!n && !p) break;

    bool zero = false;
    if (p == 1)
      zero = true;
    else if (n >= 2 && p == 2)
      zero = true;
    else if (p % 10)
    {
      switch (p)
      {
      case 10: zero = n >= 5; break;
      case 100: zero = n >= 10; break;
      case 1000: zero = n >= 15; break;
      case 10000: zero = n >= 20; break;
      case 100000:
      case 1000000: zero = n >= 25; break;
      case 10000000: zero = n >= 30; break;
      case 100000000: zero = n >= 35; break;
      case 1000000000: zero = n >= 40; break;
      }
    }

    if (zero)
    {
      std::cout << 0 << std::endl;
      continue;
    }

    // f!(n) % p
    if (n <= 20)
    {
      uint64_t fact_f = 1;
      for (int i = 2; i <= n; ++i) fact_f *= i;
      std::cout << fact_f % p << std::endl;
    }
    else
    {
      // The number of 10 base digits in 10000 factorial is 35660
      unsigned digits_qty = fact(n, digits, sizeof(digits));
      unsigned res = modulo(digits, digits_qty, p);
      std::cout << res << std::endl;
      memset(digits, 0, digits_qty);
    }
  }
#endif

  return 0;
}
