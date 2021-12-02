// -*- mode: c++; coding: utf-8 -*-
/*! \file custom.h
* \brief Декларации "специфической" реализации классов домашнего задания 13ой лекции
*/

#pragma once

#include "sketch.h"


/*! \addtogroup homework13
* @{
*/

namespace homework13 {

struct import_interface_t
{
  virtual void read(int&) = 0;
  virtual void read(unsigned&) = 0;
};

struct export_interface_t
{
  virtual void write(const int&) = 0;
  virtual void write(const unsigned&) = 0;
};

struct custom_document_t : public document_t
{
  custom_document_t();
  ~custom_document_t();
  virtual bool export_to_file(file_t*) const override;
  virtual void import_from_file(const file_t*) override;
  virtual bool is_changed() const override;
};

class custom_editor_t : public editor_t
{
public:
  static custom_editor_t& instance() { static custom_editor_t self; return self; }
  virtual document_t* internal_create_document() const override;
  virtual void internal_delete_document(document_t*) const override;

private:
  custom_editor_t() = default;
  const custom_editor_t& operator=(const custom_editor_t&) = delete;
};

}

/*! @} */
