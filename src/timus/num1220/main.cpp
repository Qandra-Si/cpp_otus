// -*- mode: c++; coding: utf-8 -*-
#include <vector>
#include <cstdint>
#include <cstdio> // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
#include <cstdlib>
#include <cstring>


struct push_t
{
  uint8_t a_b[5]; // места для хранения мало, т.ч. 0..29 бит - число, 30..39 адрес
  push_t() = default;
  push_t(const push_t& other) { memmove(a_b, other.a_b, 5); }
  push_t& operator=(const push_t& other) { memmove(a_b, other.a_b, 5); return *this; }
};

/*! \brief Задача №1220. Stacks
*
* https://acm.timus.ru/problem.aspx?space=1&num=1220
*
* Imagine, that you are employed by a software development company. You work
* now on the famous "D++ project", which is devoted to the creation of a new
* generation programming language. Your particular task is quite prosaic, though.
* You are to develop the memory manager being able to work with a large number
* of stacks.
*
* Исходные данные: The first line of the input contains the total number of
* stack operations N, 0 < N ≤ 100000. Each of the next N lines contains a
* description of a stack operation, either in the form PUSH A B (meaning to push
* B into stack A), or in the form POP A (meaning to pop an element from stack A),
* where A is the number of stack (1 ≤ A ≤ 1000), and B is an integer
* (0 ≤ B ≤ 10^9). You may assume, that every operation is correct (i.e.,
* before each POP operation, the respective stack is not empty).
*
* Результат: For each POP operation, described in the input, output the value,
* which this POP operation gets from the top of that stack, to which it is
* applied. Numbers should appear according to the order of the POP operations
* in the input. Each number should be output in a separate line.
*
* In C++, it is recommended to use stdio instead of iostream to save a reasonable
* amount of memory.
*/
int main()
{
  int n, a, b;
  char cmd[5];

  // (0.75 * 1024 * 1024 / 100000) < 8 октет на одно входное число (мало для использования stl-объектов)
  if (scanf("%d", &n) == EOF) return 0;

  using ex_push_t = union { push_t stored; uint64_t num; };
  using stacks_t = std::vector<push_t>;

  ex_push_t push;
  stacks_t stacks;
  stacks_t::reverse_iterator itr;
  stacks_t::const_reverse_iterator end;
  stacks.reserve(n);

  // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
  while ((scanf("%s%d%d", cmd, &a, &b) >= 2) && n--)
  {
    // PUSH
    if (cmd[1] == 'U')
    {
      push.num = (uint64_t)a<<30 | b;
      stacks.push_back(push.stored);
    }
    // POP
    else if (cmd[1] == 'O')
    {
      itr = stacks.rbegin();
      end = stacks.rend();
      push.num = 0;
      int _a, _b;
      for ( ; itr != end; ++itr)
      {
        push.stored = *itr;
        _a = (uint64_t)push.num >> 30;
        if (_a == a)
        {
          _b = push.num & 0x3fffffff;
          printf("%d\n", (int)_b);
          stacks.erase(std::next(itr).base());
          break;
        }
      }
    }
  }
  return 0;
}
