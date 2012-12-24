// mouse.cpp
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

#include "dim/core/mouse.hpp"

#include <GL/glfw.h>

namespace dim
{

	Mouse::Mouse()
	:
	  d_lButton(false),
	  d_rButton(false),
	  d_lPressed(false),
	  d_rPressed(false),
	  d_lRelease(false),
	  d_rRelease(false)
	{
	}
	
	bool Mouse::lPressed() const
	{
	  return d_lPressed;
	}
	
  bool Mouse::rPressed() const
	{
	  return d_rPressed;
	}
	
	bool Mouse::lRelease() const
	{
	  return d_lRelease;
	}
	
	bool Mouse::rRelease() const
	{
	  return d_rRelease;
	}
	
  glm::ivec2 const &Mouse::coor() const
  {
    return d_coor;
  }

  void Mouse::update()
  {
  	d_lRelease = false;
  	d_rRelease = false;
    glfwGetMousePos(&d_coor.x, &d_coor.y);
  
    d_lPressed = (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    d_rPressed = (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
  
    if(d_lPressed == false)
    {
      if(d_lButton == true)
      {
        d_lRelease = true;
        d_lButton = false;
      }
    }
    else
    {
      d_lButton = true;
    }
  
    if(d_rPressed == false)
    {
      if(d_rButton == true)
      {
        d_rRelease = true;
        d_rButton = false;
      }
    }
    else
    {
      d_rButton = true;
    }
  }
}
