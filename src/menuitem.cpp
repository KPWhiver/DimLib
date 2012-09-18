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

#include "dim/menuitem.hpp"
#include "dim/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace std;
using namespace glm;

namespace dim
{

	MenuItem::MenuItem(string const &text)
	:
	    d_context(0),
			d_strText(text),
			d_selected(false),
      d_width(0),
      d_height(0)
	{
	}
	
	MenuItem::MenuItem()
	:
	    d_context(0),
			d_selected(false),
      d_width(0),
      d_height(0)
	{
	}

	void MenuItem::setContext(Context *context)
	{
		d_context = context;
		d_text = Texture<GLubyte>(d_context->font().generateTexture(d_strText, true, d_width, d_height, Filtering::linear));
	}

	void MenuItem::setSize(size_t width, size_t height)
	{
		d_width = width;
		d_height = height;
	}

	bool MenuItem::listen(int x, int y, dim::Mouse const &mouse)
  {
    ivec2 mouseC = mouse.coor();
  
    if(mouseC.x > x && mouseC.x < x + static_cast<int>(d_width) && mouseC.y > y && mouseC.y < y + static_cast<int>(d_height))
		{
		  if(mouse.lRelease())
		  {
		  	if(d_listenerFunction)
		  		d_listenerFunction();
		  		
		  	return true;
		  }
		  d_selected = true;
		}
		else
		  d_selected = false;
		
		return false;
  }

  void MenuItem::draw(int x, int y, Texture<> const &texture, Texture<> const &hoverTexture)
  {
	  if(d_context == 0)
	    return;

		if(d_selected == true)
		  d_context->shader().set("in_texture0", hoverTexture, 0);
		else
		  d_context->shader().set("in_texture0", texture, 0);

		d_context->shader().set("in_text", d_text, 1);

		mat4 modelMatrix(1.0);
    modelMatrix = translate(modelMatrix, vec3(x, y, 0));
	  modelMatrix = scale(modelMatrix, vec3(d_width, d_height, 1.0));
	  d_context->shader().set("in_mat_model", modelMatrix);

	  d_context->mesh().draw();
  }

  void MenuItem::setListener(function<void(void)> const &listenerFunction)
  {
  	d_listenerFunction = listenerFunction;
  }
}
