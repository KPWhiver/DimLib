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

#include "DIM/menu.hpp"

#include <iostream>

namespace dim
{

Menu::Menu(size_t width, size_t height)
:
  d_width(width),
  d_height(height),
  d_active(false),
  d_x(0),
  d_y(0)
{
	d_priority = 100;
}

Menu::Menu()
:
  d_width(10),
  d_height(10),
  d_active(false),
  d_x(0),
  d_y(0)
{
	d_priority = 100;
}

bool Menu::listen()
{
  if(d_active == true)
  {
    for(size_t idx = 0; idx != d_items.size(); ++idx)
    {
      if(d_items[idx]->listen(d_x, d_y - idx * d_height - d_height, d_width, d_height))
      {
        d_active = false;
        return true;
      }
    }
  }
  return false;
}

void Menu::draw()
{
  if(d_active == true)
  {
    for(size_t idx = 0; idx != d_items.size(); ++idx)
    {
      d_items[idx]->draw(d_x, d_y - idx * d_height - d_height, d_width, d_height);
    }
  }
}

void Menu::add(MenuItem *item)
{
  d_items.push_back(item);
  item->setSize(d_width, d_height);
}

void Menu::setContext(Context *context)
{
	d_context = context;
	for(size_t idx = 0; idx != d_items.size(); ++idx)
	{
		d_items[idx]->setContext(context);
	}
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

}
