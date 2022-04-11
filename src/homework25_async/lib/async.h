// -*- mode: c++; coding: utf-8 -*-
/*! \file async.h
* \brief Интерфейс асинхронного ввода данныкоманд для домашнего задания 25ой лекции
*/

#pragma once

#include <cpp_otus_config.h>

#include <functional>
#include <ctime>



/*! \addtogroup homework25
* @{
*/

#if defined(CPP_OTUS_ALL_DYN_LINK) || defined(CPP_OTUS_ASYNC_DYN_LINK)
#  if defined( CPP_OTUS_ASYNC_SOURCE )
#    define CPP_OTUS_ASYNC_DECL CPP_OTUS_SYMBOL_EXPORT
#  else
#    define CPP_OTUS_ASYNC_DECL CPP_OTUS_SYMBOL_IMPORT
#    define CPP_OTUS_DYN_LINK
#  endif
#endif

#ifndef CPP_OTUS_ASYNC_DECL
#  define CPP_OTUS_ASYNC_DECL
#endif

namespace homework25 {

struct async_ctx_t;
using commands_finalizer_t = std::function<void(const std::time_t& t, const std::string& line)>;

CPP_OTUS_ASYNC_DECL async_ctx_t* connect(int n, const commands_finalizer_t& finalizer);
CPP_OTUS_ASYNC_DECL void receive(async_ctx_t* ctx, const std::string& cmd);
CPP_OTUS_ASYNC_DECL void disconnect(async_ctx_t* ctx);

}

/*! @} */
