// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <algorithm>

#include "application_specific.h"


namespace homework13 {

//------------------------------------------------------------------------------
// custom_circle_t
//------------------------------------------------------------------------------
custom_circle_t::custom_circle_t(int x, int y, unsigned radius) :
  circle_t(x, y, radius),
  primitive_drawer_t()
{
}

void custom_circle_t::export_to_file(file_interface_t* ofile)
{
  const auto prms = get_params();
  std::cout << "Circle numeric params (x, y, radius): ";
  ofile->write(std::get<0>(prms));
  ofile->write(std::get<1>(prms));
  ofile->write(std::get<2>(prms));
  std::cout << std::endl;
}

void custom_circle_t::import_from_file(const file_interface_t* ifile)
{
  std::cout << "Please input circle numeric params (x, y, radius): ";
  ifile->read(x);
  ifile->read(y);
  ifile->read(radius);
}

void custom_circle_t::draw()
{
  const auto prms = get_params();
  std::cout
    << "circle x=" << std::get<0>(prms)
    << ",y=" << std::get<1>(prms)
    << ",r=" << std::get<2>(prms);
}

//------------------------------------------------------------------------------
// custom_rectangle_t
//------------------------------------------------------------------------------
custom_rectangle_t::custom_rectangle_t(int left, int top, unsigned width, unsigned height) :
  rectangle_t(left, top, width, height),
  primitive_drawer_t()
{
}

void custom_rectangle_t::export_to_file(file_interface_t* ofile)
{
  const auto prms = get_params();
  std::cout << "Rectangle numeric params (left, top, width, height): ";
  ofile->write(std::get<0>(prms));
  ofile->write(std::get<1>(prms));
  ofile->write(std::get<2>(prms));
  ofile->write(std::get<3>(prms));
  std::cout << std::endl;
}

void custom_rectangle_t::import_from_file(const file_interface_t* ifile)
{
  std::cout << "Please input rectangle numeric params (left, top, width, height): ";
  ifile->read(left);
  ifile->read(top);
  ifile->read(width);
  ifile->read(height);
}

void custom_rectangle_t::draw()
{
  const auto prms = get_params();
  std::cout
    << "rect l=" << std::get<0>(prms)
    << ",t=" << std::get<1>(prms)
    << ",w=" << std::get<2>(prms)
    << ",h=" << std::get<3>(prms);
}

//------------------------------------------------------------------------------
// custom_model_t
//------------------------------------------------------------------------------
custom_circle_t* custom_model_t::add_circle(int x, int y, unsigned radius)
{
  circles.emplace_back(new custom_circle_t(x, y, radius));
  return circles.back().get();
}

custom_rectangle_t* custom_model_t::add_rectangle(int left, int top, unsigned width, unsigned height)
{
  rectangles.emplace_back(new custom_rectangle_t(left, top, width, height));
  return rectangles.back().get();
}

void custom_model_t::move(primitive_t* const primitive, int offset_x, int offset_y)
{
  // ну тут по хорошему надо бы проверить, есть ли примитив в наборах данных модели?
  // наша простейшая реализация этого делать не будет (доверяем вызывающему коду
  primitive->move(offset_x, offset_y);
}

void custom_model_t::remove(primitive_t* const primitive)
{
  for (auto itr = circles.begin(), end = circles.end(); itr != end; ++itr)
  {
    if (itr->get() != primitive) continue;
    circles.erase(itr);
    return;
  }
  for (auto itr = rectangles.begin(), end = rectangles.end(); itr != end; ++itr)
  {
    if (itr->get() == primitive) continue;
    rectangles.erase(itr);
    return;
  }
}

//------------------------------------------------------------------------------
// console_view_t
//------------------------------------------------------------------------------
void console_view_t::attach_model(custom_model_t* model)
{
  this->model = model;
}

void console_view_t::detach_model()
{
  model = nullptr;
}

void console_view_t::render()
{
  if (!model) return;
  std::cout << "RENDER:";
  // с этим множественным наследованием получилось как через одно место :(
  // ну для circle+rectangle сгодится, а в любом другом случае так делать не комильфо
  // ... с другой стороны у нас тут простейший графический редактор с малым кол-вом типов примитивов
  auto lambda = [](primitive_drawer_t* p) { std::cout << " ["; p->draw(); std::cout << "]"; };
  for (auto& p : model->get_circles()) lambda(dynamic_cast<primitive_drawer_t*>(p.get()));
  for (auto& p : model->get_rectangles()) lambda(dynamic_cast<primitive_drawer_t*>(p.get()));
  std::cout << std::endl;
}

//------------------------------------------------------------------------------
// console_file_t
//------------------------------------------------------------------------------
void console_file_t::open(const char* disclaimer)
{
  std::cout << disclaimer;
}

void console_file_t::close()
{
  std::cout << std::endl;
}

void console_file_t::read(int& value) const
{
  std::cin >> value;
}

void console_file_t::read(unsigned& value) const
{
  std::cin >> value;
}

void console_file_t::write(const int& value)
{
  std::cout << " " << value;
}

void console_file_t::write(const unsigned& value)
{
  std::cout << " " << value;
}

//------------------------------------------------------------------------------
// custom_document_t
//------------------------------------------------------------------------------
bool custom_document_t::export_to_file(file_interface_t* ofile) const
{
  std::cout
    << "This document has "
    << model.get_circles().size() << " circles and "
    << model.get_rectangles().size() << " rectangles. Let's export them." << std::endl;

  for (const auto& c : model.get_circles())
    c.get()->export_to_file(ofile);
  for (const auto& r : model.get_rectangles())
    r.get()->export_to_file(ofile);

  return true;
}

void custom_document_t::import_from_file(const file_interface_t* ifile)
{
  std::cout << "New empty document created..." << std::endl;
  while (true)
  {
    std::cout << "Do you want to add a new primitive (0 - no, 1 - yes)? ";

    int answer;
    ifile->read(answer);
    if (!answer) return;

    std::cout << "What type of primitive to add (0 - circle, 1 - rectangle)? ";
    ifile->read(answer);

    primitive_t* primitive; // нам также всё равно какой именно примитив будет создан, у примитивов одинаковый интерфейс

    switch (answer)
    {
    case 0: primitive = model.add_circle(0, 0, 0); break;
    case 1: primitive = model.add_rectangle(0, 0, 0, 0); break;
    default: return;
    }

    primitive->import_from_file(ifile);
  }
}

//------------------------------------------------------------------------------
// custom_editor_t
//------------------------------------------------------------------------------
custom_editor_t::custom_editor_t() : editor_t()
{
  view.reset(new console_view_t());
}

console_view_t* custom_editor_t::get_view()
{
  return view.get();
}

custom_model_t* custom_editor_t::get_model()
{
  return &reinterpret_cast<custom_document_t*>(get_document())->model;
}

document_t* custom_editor_t::internal_create_document() const
{
  custom_document_t * document = new custom_document_t();
  view->attach_model(&document->model);
  return document;
}

void custom_editor_t::internal_delete_document(document_t* document) const
{
  delete reinterpret_cast<custom_document_t*>(document);
}

void custom_editor_t::render()
{
  get_view()->render();
}

primitive_t* custom_editor_t::add_circle(int x, int y, unsigned radius)
{
  return get_model()->add_circle(x, y, radius);
}

primitive_t* custom_editor_t::add_rectangle(int left, int top, unsigned width, unsigned height)
{
  return get_model()->add_rectangle(left, top, width, height);
}

void custom_editor_t::move(primitive_t* const primitive, int offset_x, int offset_y)
{
  get_model()->move(primitive, offset_x, offset_y);
}

void custom_editor_t::remove(primitive_t* const primitive)
{
  get_model()->remove(primitive);
}

}
