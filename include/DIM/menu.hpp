// menu.hpp
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

#ifndef MENU_HPP
#define MENU_HPP

#include "DIM/texture.hpp"
#include "DIM/menuitem.hpp"
#include "DIM/component.hpp"

#include <memory>

namespace dim
{
	class Menu : public Component
	{
		size_t d_width, d_height;
		int d_x, d_y;
		
		std::vector<MenuItem*> d_items;
    bool d_active;
      
  public:
    Menu(size_t d_width, size_t d_height);
    Menu();

    void draw(int x, int y);
    bool listen(int x, int y, dim::Mouse const &mouse);
    void add(MenuItem *item);
    void activate(int x, int y);
    void deactivate();
    bool active() const;
    virtual void setContext(Context *context);

	};

}

#endif /* BUTTON_HPP_ */
