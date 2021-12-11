// -*- mode: c++; coding: utf-8 -*-
#include <cstdio> // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
#include <cstdlib>
#include <cstring>


#pragma pack(push, 1)
struct push_t
{
  unsigned short a;
  unsigned int b;
};
#pragma pack(pop)

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
  #define MAX_N 100000

  int n, capacity;
  unsigned int a, b;
  char cmd[5];
  push_t stacks[MAX_N];
  const push_t * stacks_end = &stacks[MAX_N];
  push_t * pbegin = stacks, *pcursor = stacks;

  // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
  // (0.75 * 1024 * 1024 / 100000) < 8 октет на одно входное число (мало для использования stl-объектов)
  // empty program requires ~200K

  if (scanf("%d", &n) == EOF) return 0;

  while ((scanf("%s%u", cmd, &a) != EOF))
  {
    // PUSH
    if (cmd[1] == 'U')
    {
      if (scanf("%u", &b) == EOF) return 0;
      if (pcursor == stacks_end) // значит begin_cursor передвинут! двигаем блок данных в начало
      {
        capacity = pcursor - pbegin;
        memmove(stacks, pbegin, sizeof(push_t) * capacity);
        pbegin = stacks;
        pcursor = pbegin + capacity;
      }
      *pcursor++ = push_t{(unsigned short)a, b};
    }
    // POP
    else if (cmd[1] == 'O')
    {
      for (push_t *p = pcursor - 1; p >= pbegin; --p)
      {
        if (p->a == a)
        {
          printf("%u\n", p->b);
          if (p == pbegin)
            pbegin++;
          else if (p == (pcursor-1))
            pcursor--;
          else
          {
            memmove(p, p+1, (pcursor-1-p)*sizeof(push_t));
            pcursor--;
          }
          // посольку голова тоже двигается, и если она догнала хвост, то ставим их в начало
          if (pcursor == pbegin)
          {
            pcursor = pbegin = stacks;
          }
          break;
        }
      }
    }
    //debug:printf("current: "); for (int i = 0; i < MAX_N; ++i) printf("%u %u, ", stacks[i].a, stacks[i].b); printf("; begin=%p end=%p\n", pbegin-stacks, pcursor-stacks);
    if (--n == 0) break;
  }
  return 0;
}
