// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include "custom.h"


//------------------------------------------------------------------------------
// custom_document_t
//------------------------------------------------------------------------------
bool custom_document_t::export_to_file(file_t*) const
{
  return true;
}

void custom_document_t::import_from_file(const file_t*)
{
}

bool custom_document_t::is_changed() const
{
  return false;
}

//------------------------------------------------------------------------------
// custom_editor_t
//------------------------------------------------------------------------------
document_t* custom_editor_t::internal_create_document() const
{
  return new custom_document_t();
}

void custom_editor_t::internal_delete_document(document_t* document) const
{
  delete reinterpret_cast<custom_document_t*>(document);
}


////////////////////////////////////////////////////////////////////////////////
custom_document_t::custom_document_t() : document_t()
{
  std::cout << "ctor document" << std::endl;
}
custom_document_t::~custom_document_t()
{
  std::cout << "dtor document" << std::endl;
}
