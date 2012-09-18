// menu.cpp
//
// Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "dim/menu.hpp"

#include <iostream>

namespace dim
{

Menu::Menu(size_t width, size_t height)
:
  d_width(width),
  d_height(height),
  d_x(0),
  d_y(0),
  d_active(false)
{
	d_priority = 100;
}

Menu::Menu()
:
  //Menu(10, 10)
  d_width(10),
  d_height(10),
  d_x(0),
  d_y(0),
  d_active(false)
{
	d_priority = 100;
}

bool Menu::listen(int x, int y, dim::Mouse const &mouse)
{
  if(d_active == true)
  {
    for(size_t idx = 0; idx != d_items.size(); ++idx)
    {
      if(d_items[idx]->listen(x + d_x, y + d_y - idx * d_height - d_height, mouse))
      {
        d_active = false;
        return true;
      }
    }
  }
  return false;
}

void Menu::draw(int x, int  y)
{
  if(d_active == true)
  {
    for(size_t idx = 0; idx != d_items.size(); ++idx)
      d_items[idx]->draw(x + d_x, y + d_y - idx * d_height - d_height, d_context->buttonTexture(), d_context->buttonHoverTexture());
  }
}

void Menu::add(MenuItem *item)
{
  item->setSize(d_width, d_height);
  d_items.push_back(item);
}

void Menu::setContext(Context *context)
{
	d_context = context;
	for(size_t idx = 0; idx != d_items.size(); ++idx)
		d_items[idx]->setContext(context);
}

void Menu::activate(int x, int y)
{
	d_x = x;
	d_y = y;
  d_active = true;
}

void Menu::deactivate()
{
	d_active = false;
}

bool Menu::active() const
{
	return d_active;
}

Component *Menu::v_clone() const
{
  return new Menu(*this);
}


}
