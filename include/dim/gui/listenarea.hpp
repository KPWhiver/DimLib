// listenarea.hpp
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

#ifndef LISTENAREA_HPP
#define LISTENAREA_HPP

#include "dim/core/texture.hpp"
#include "dim/gui/component.hpp"

#include <functional>

namespace dim
{
	class ListenArea : public Component
	{
		int d_x, d_y;
		size_t d_width, d_height;
		
		std::function<void(void)> d_listenerFunctionLeftRelease;
		std::function<void(void)> d_listenerFunctionLeftPressed;
		std::function<void(void)> d_listenerFunctionRightRelease;
		std::function<void(void)> d_listenerFunctionRightPressed;

  public:
    ListenArea(int x, int y, size_t width, size_t height);
    ListenArea();
    bool listen(int x, int y, dim::Mouse const &mouse);
    void size();
    void setListenerLeftRelease(std::function<void(void)> const &listenerFunction);
    void setListenerLeftPressed(std::function<void(void)> const &listenerFunction);
    void setListenerRightRelease(std::function<void(void)> const &listenerFunction);
    void setListenerRightPressed(std::function<void(void)> const &listenerFunction);

  private:
    virtual Component *v_clone() const;
	};

}

#endif /* BUTTON_HPP_ */
