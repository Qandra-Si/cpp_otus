// -*- mode: c++; coding: utf-8 -*-
#include <core/utils.h>

#include "interface.h"
#include "async.h"


namespace homework25
{

struct async_ctx_t : protected homework15::commands_finalizer_t
{
  homework15::custom_bulk_t bulk;
  async_ctx_t(unsigned n) : bulk(n, this) { }
protected:
  virtual void finish(const std::time_t& t, const std::string& line) override
  {
  }
};

async_ctx_t* connect(int n)
{
  async_ctx_t* ctx = new async_ctx_t(n);
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
