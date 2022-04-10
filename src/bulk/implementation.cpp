// -*- mode: c++; coding: utf-8 -*-

#include <cassert>

#include "interface.h"


namespace homework15 {

//------------------------------------------------------------------------------
// custom_bulk_t
//------------------------------------------------------------------------------
custom_bulk_t::custom_bulk_t(unsigned n, const commands_processor_t::finalizer_t& finalizer) :
  bulk_size(n),
  num_nested_transactions(0),
  delayed_commands(finalizer)
{
}

custom_bulk_t::~custom_bulk_t()
{
  if (num_nested_transactions == 0)
  {
    delayed_commands.run();
  }
}

void custom_bulk_t::prepare(abstract_command_t* const cmd)
{
  delayed_commands.add(cmd);

  if (num_nested_transactions) return;
  if (delayed_commands.bulk_size() < bulk_size) return;
  delayed_commands.run();
}

unsigned custom_bulk_t::start_transaction()
{
  if (!num_nested_transactions)
  {
    delayed_commands.run();
  }
  return ++num_nested_transactions;
}

unsigned custom_bulk_t::commit_transaction()
{
  assert(("There are no any started transaction", num_nested_transactions > 0));
  if (--num_nested_transactions == 0)
  {
    delayed_commands.run();
  }
  return num_nested_transactions;
}

//------------------------------------------------------------------------------
// custom_command_t
//------------------------------------------------------------------------------
custom_command_t::custom_command_t(const std::string& cmd) : std::string(cmd)
{
}

std::string custom_command_t::execute()
{
  return *this;
}

//------------------------------------------------------------------------------
// commands_processor_t
//------------------------------------------------------------------------------
void commands_processor_t::add(abstract_command_t* const _cmd)
{
  custom_command_t* cmd = dynamic_cast<custom_command_t*>(_cmd);
  assert(("There are no any other command's implementation other than this", cmd));

  if (empty()) t = std::time(0);
  emplace_back(value_type(cmd));
}

void commands_processor_t::run()
{
  if (empty()) return;

  std::string line;
  do
  {
    if (line.empty())
      line = "bulk: ";
    else
      line += ", ";
    line += front().get()->execute();
    erase(begin());
  } while (!empty());

  finalizer(t, line);
}


}
