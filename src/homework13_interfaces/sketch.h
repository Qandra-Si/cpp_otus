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

class import_interface_t;
class export_interface_t;
class file_t;

/*! \brief Графический примитив
*
* Просто некая абстрактная сущность, которая умеет куда-то двигаться, а также
* экспортироваться и импортироваться. Ни от чего не зависит, но использует
* интерфейсы экспорта и импорта.
*/
struct primitive_t
{
  virtual void export_to_file(export_interface_t*) = 0;
  virtual void import_from_file(const import_interface_t*) = 0;
  virtual void move(int offset_x, int offset_y) = 0;
};

/*! \brief Графический примитив "окружность"
*
* Реализует метод перемещения примитива \ref primitive_t, а также умеет
* создаваться в указанной позиции, как и сообщать своё местоположение.
*
* - \see primitive_t
* - \see rectangle_t
*/
class circle_t : public primitive_t
{
protected:
  int x, y;
  unsigned radius;
public:
  circle_t(int x, int y, unsigned radius) : x(x), y(y), radius(radius) { }
  std::tuple<int, int, unsigned> get_params() { return std::make_tuple(x, y, radius); }
  virtual void move(int offset_x, int offset_y) override { x += offset_x; y += offset_y; }
};

/*! \brief Графический примитив "прямоугольник"
*
* Реализует метод перемещения примитива \ref primitive_t, а также умеет
* создаваться в указанной позиции, как и сообщать своё местоположение.
*
* - \see primitive_t
* - \see circle_t
*/
class rectangle_t : public primitive_t
{
protected:
  int left, top;
  unsigned width, height;
public:
  rectangle_t(int left, int top, unsigned width, unsigned height) : left(left), top(top), width(width), height(height) { }
  std::tuple<int, int, unsigned, unsigned> get_params() { return std::make_tuple(left, top, width, height); }
  virtual void move(int offset_x, int offset_y) override { left += offset_x; top += offset_y; }
};

/*! \brief Документ с графическими примитивами
*
* Просто некая абстрактная сущность, которая умеет экспортироваться и
* импортироваться. Ни от чего не зависит, но использует интерфейсы экспорта и
* импорта.
*
* Предполагается, что документ состоит из графических примитивов \ref primitive_t,
* но из интерфейса документа это вовсе не следует. Доверим подобную компоновку
* model-view-controller шаблону проектирования.
*
* - \see primitive_t
* - \see controller_t
*/
class document_t
{
public:
  virtual bool export_to_file(file_t*) const = 0;
  virtual void import_from_file(const file_t*) = 0;
protected:
  virtual ~document_t() = default;
};

/*! \brief Контроллер (controller) для управления model и view
*
* Просто некая абстрактная сущность, которая умеет что-то где-то рендерить, и
* добавлять, удалять и перемещать графические примитивы \ref primitive_t. Это
* просто интерфейс, который должен будет реализовать \ref editor_t.
*
* Предполагается, что контроллер будет взаимодействовать с model и view
* сущностями, но из данной декларации этого не следует и соответственно
* "развязывает руки", так чтобы конкретика о том как и что компонуется и
* отображается появилась потом, в custom-реализации.
*
* - \see primitive_t
* - \see editor_t
*/
struct controller_t
{
public: // методы controller-а, который взаимодействуют с сущностью view (отображение примитивов)
  virtual void render() = 0;
public: // методы controller-а, которые взаимодействуют с сущностью model (управление примитивами)
  virtual primitive_t* add_circle(int x, int y, unsigned radius) = 0;
  virtual primitive_t* add_rectangle(int left, int top, unsigned width, unsigned height) = 0;
  virtual void move(primitive_t* const primitive, int offset_x, int offset_y) = 0;
  virtual void remove(primitive_t* const primitive) = 0;
};

/*! \brief Простейший графический векторный редактор
*
* Это модель SDI-приложения, которая умеет работать с одним документом
* \ref document_t и требует к реализации интерфейс контроллера \ref controller_t.
*
* Графический редактор ничего не знает о том, какую именно реализацию документа
* ему придётся создавать, всё отдано на откуп custom-реализации, но зато редактор
* умеет пересодавать документ и обращаться к его методами импорта и экспорта.
*
* - \see document_t
* - \see controller_t
*/
class editor_t : public controller_t
{
public:
  document_t* get_document() { return document.get(); }
  document_t* create_document() {
    auto deleter = [this](document_t* d) { this->internal_delete_document(d); };
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
