// menuitem.cpp
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

#include "DIM/menuitem.hpp"
#include "DIM/window.hpp"
#include "DIM/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace std;
using namespace glm;

namespace dim
{
	MenuItem::MenuItem(string const &text)
	:
			d_width(0),
			d_height(0),
			d_text(0),
			d_strText(text)

	{
		d_priority = 101;
	}
	
	MenuItem::MenuItem()
	:
			d_width(0),
			d_height(0),
			d_text(0)
	{
		d_priority = 101;
	}
	
	MenuItem::~MenuItem()
	{
	  delete d_text;
	}

	void MenuItem::setContext(Context *context)
	{
		d_context = context;
		d_text = new Texture(d_context->font().generateTexture(d_strText, d_width, d_height));
	}

	void MenuItem::setSize(size_t width, size_t height)
	{
		d_width = width;
		d_height = height;
	}

	bool MenuItem::listen(int x, int y, size_t width, size_t height)
  {
    ivec2 mouse = d_context->mouse().coor();
  
    if(mouse.x > x && mouse.x < x + static_cast<int>(width) && mouse.y > y && mouse.y < y + static_cast<int>(height))
		{
		  if(d_context->mouse().lRelease())
		  {
		  	if(d_listenerFunction)
		  		d_listenerFunction();
		  		
		  	return true;
		  }
		}
		return false;
  }

  void MenuItem::draw(int x, int y, size_t width, size_t height)
  {
	  if(d_context == 0)
	    return;

	  ivec2 mouse = d_context->mouse().coor();

		if(mouse.x > x && mouse.x < x + static_cast<int>(width) && mouse.y > y && mouse.y < y + static_cast<int>(height))
		  d_context->buttonHoverTexture().send(0, "in_texture0");
		else
		  d_context->buttonTexture().send(0, "in_texture0");

    Shader::in_mat_model() = translate(Shader::in_mat_model(), vec3(x, y, 0));
	  Shader::in_mat_model() = scale(Shader::in_mat_model(), vec3(width/10.0, height/10.0, 1.0));
	  Shader::active().send(Shader::in_mat_model(), "in_mat_model");

    Shader::active().transformBegin();
	  //mesh.draw();
	  d_context->mesh().draw();

	  d_text->send(0, "in_texture0");

	  d_context->mesh().draw();

	  Shader::active().transformEnd();
  }

  void MenuItem::setListener(function<void(void)> const &listenerFunction)
  {
  	d_listenerFunction = listenerFunction;
  }
}
