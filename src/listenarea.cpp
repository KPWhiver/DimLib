// listenarea.cpp
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

#include "dim/listenarea.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace glm;

namespace dim
{

	ListenArea::ListenArea(int x, int y, size_t width, size_t height)
	:
			d_x(x),
			d_y(y),
			d_width(width),
			d_height(height)
	{
	}
	
	ListenArea::ListenArea()
	:
			d_x(0),
			d_y(0),
			d_width(10),
			d_height(10)
	{
	}

	bool ListenArea::listen(int x, int y, dim::Mouse const &mouse)
	{	  


	  if(d_context == 0)
	    return false;
	
	  ivec2 mouseC = mouse.coor();
	  
	  x += d_x;
	  y += d_y;
	  
		if(mouseC.x > x && mouseC.x < x + static_cast<int>(d_width) && mouseC.y > y && mouseC.y < y + static_cast<int>(d_height))
		{

		  if(mouse.lRelease())
		  {
		  	if(d_listenerFunctionLeftRelease)
		  		d_listenerFunctionLeftRelease();
		  }
		  else if(mouse.rRelease())
		  {
		    if(d_listenerFunctionRightRelease)
		  		d_listenerFunctionRightRelease();
		  }
		  else if(mouse.lPressed())
		  {
		  	if(d_listenerFunctionLeftPressed)
		  		d_listenerFunctionLeftPressed();
		  }
		  else if(mouse.rPressed())
		  {
		    if(d_listenerFunctionRightPressed)
		  		d_listenerFunctionRightPressed();
		  }
		  else
		  {
		    return false;
		  }
		  return true;
		}		
		return false;
	}

	void ListenArea::setListenerLeftRelease(std::function<void(void)> const &listenerFunction)
	{
		d_listenerFunctionLeftRelease = listenerFunction;
	}
	
	void ListenArea::setListenerLeftPressed(std::function<void(void)> const &listenerFunction)
	{
		d_listenerFunctionLeftPressed = listenerFunction;
	}
	
	void ListenArea::setListenerRightRelease(std::function<void(void)> const &listenerFunction)
	{
		d_listenerFunctionRightRelease = listenerFunction;
	}
	
	void ListenArea::setListenerRightPressed(std::function<void(void)> const &listenerFunction)
	{
		d_listenerFunctionRightPressed = listenerFunction;
	}

}
