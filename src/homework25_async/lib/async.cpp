// -*- mode: c++; coding: utf-8 -*-
#include <core/utils.h>

#include <interface.h>
#include "async.h"


namespace homework25
{

//------------------------------------------------------------------------------
// async_ctx_t
//------------------------------------------------------------------------------
struct async_ctx_t
{
  homework15::custom_bulk_t bulk;
  async_ctx_t() = delete;
  async_ctx_t(unsigned n, const commands_finalizer_t& finalizer) : bulk(n, finalizer) { }
};

//------------------------------------------------------------------------------
// connect, receive, disconnect
//------------------------------------------------------------------------------
async_ctx_t* connect(int n, const commands_finalizer_t& finalizer)
{
  async_ctx_t* ctx = new async_ctx_t(n, finalizer);
  return ctx;
}

void receive(async_ctx_t* ctx, const std::string& cmd)
{
  if (cmd.empty())
    return;
  else if (cmd == "{")
    ctx->bulk.start_transaction();
  else if (cmd == "}")
    ctx->bulk.commit_transaction();
  else
    ctx->bulk.prepare(new homework15::custom_command_t(cmd));
}

void disconnect(async_ctx_t* ctx)
{
  delete ctx;
}

}
