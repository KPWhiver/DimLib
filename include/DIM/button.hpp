// button.hpp
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

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "DIM/texture.hpp"
#include "DIM/menu.hpp"
#include "DIM/component.hpp"

#include <functional>
#include <string>

namespace dim
{
	class Button : public Component
	{
		int d_x, d_y;
		size_t d_width, d_height;
		std::function<void(void)> d_listenerFunction;
		Texture *d_text;
		std::string d_strText;
		
		Menu *d_menu;

  public:
    Button(int x, int y, size_t width, size_t height, std::string const &text);
    Button();
    ~Button();
    void draw();
    bool listen();
    void size();
    void setListener(std::function<void(void)> const &listenerFunction);
    void addMenu(Menu *menu);
    void setContext(Context *context);

	};

}

#endif /* BUTTON_HPP_ */
