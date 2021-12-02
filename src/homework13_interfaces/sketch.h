// -*- mode: c++; coding: utf-8 -*-
/*! \file sketch.h
* \brief Макет классов для выполнения домашнего здаания 13ой лекции
*/

#pragma once

#include <memory>
#include <tuple>
#include <functional>


/*! \defgroup homework13 Макет классов для сборки эскизного приложения "Простейший графический векторный редактор"
*
* Подробное описание назначения классов см. в корневом README.md файле.
*/

/*! \addtogroup homework13
* @{
*/

namespace homework13 {

struct import_interface_t;
struct export_interface_t;

struct primitive_t
{
  virtual void export_to_file(import_interface_t*) = 0;
  virtual void import_from_file(export_interface_t*) = 0;
  virtual void move(int offset_x, int offset_y) = 0;
};

class circle_t : public primitive_t
{
  int x, y;
  unsigned radius;
public:
  std::tuple<int, int, unsigned> get_params() { return std::make_tuple(x, y, radius); }
};

class rectangle_t : public primitive_t
{
  int left, top;
  unsigned width, height;
public:
  std::tuple<int, int, unsigned, unsigned> get_params() { return std::make_tuple(left, top, width, height); }
};

class file_t;
class editor_t;

class document_t
{
public:
  virtual bool export_to_file(file_t*) const = 0;
  virtual void import_from_file(const file_t*) = 0;
  virtual bool is_changed() const = 0;
protected:
  virtual ~document_t() = default;
};

class editor_t
{
public:
  document_t* get_document() { return document.get(); }
  document_t* create_document() {
    auto deleter = [this](document_t* d) { internal_delete_document(d); };
    document = std::unique_ptr<document_t, std::function<void(document_t*)>>(internal_create_document(), deleter);
    return get_document();
  }
  void import_from_file(const file_t* file) { create_document(); document->import_from_file(file); }
  bool export_to_file(file_t* file) { return document ? document->export_to_file(file) : false; }
protected:
  virtual ~editor_t() = default;
private:
  std::unique_ptr<document_t, std::function<void(document_t*)>> document;
  virtual document_t* internal_create_document() const = 0;
  virtual void internal_delete_document(document_t*) const = 0;
};

}

/*! @} */
