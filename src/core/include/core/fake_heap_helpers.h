// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <iostream>


namespace core {

/*! \brief Отладчик фейковой кучи \ref fake_heap_t
 */
class fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char> & get() = 0;
};

}
