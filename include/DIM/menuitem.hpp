// menuitem.hpp
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

#ifndef MENUITEM_HPP
#define MENUITEM_HPP

#include "DIM/texture.hpp"
#include "DIM/component.hpp"

#include <functional>
#include <string>

namespace dim
{
	class MenuItem : public Component
	{
		size_t d_width;
		size_t d_height;
		std::function<void(void)> d_listenerFunction;
		Texture *d_text;
		std::string d_strText;

  public:
    MenuItem(std::string const &text);
    MenuItem();
    ~MenuItem();


    void draw(int x, int y, size_t width, size_t height);
    bool listen(int x, int y, size_t width, size_t height);
    void setListener(std::function<void(void)> const &listenerFunction);
    void setSize(size_t width, size_t height);
    void setContext(Context *context);
	};

}

#endif /* BUTTON_HPP_ */
