// switch.cpp
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

#include "dim/gui/switch.hpp"
#include "dim/core/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace glm;
using namespace std;

namespace dim
{
	Switch::Switch(int x, int y, size_t width, size_t height, string const &text)
	:
			d_x(x),
			d_y(y),
			d_width(width),
			d_height(height),
			d_strText(text),
			d_enabled(false),
			d_selected(false)
	{
		d_priority = 50;
	}
	
	Switch::Switch()
	:
			d_x(0),
			d_y(0),
			d_width(10),
			d_height(10),
			d_enabled(false),
			d_selected(false)
	{
		d_priority = 50;
	}
	
	void Switch::setContext(Context *context)
	{
		d_context = context;
	  d_text = Texture<GLubyte>(d_context->font().generateTexture(d_strText, true, d_width, d_height, Filtering::linear));
	}
	
	bool Switch::listen(int x, int y, glm::ivec2 const &mouse)
	{
	  x += d_x;
	  y += d_y;

		if(mouse.x > x && mouse.x < x + static_cast<int>(d_width) && mouse.y > y && mouse.y < y + static_cast<int>(d_height))
		{
			if(true) //TODO: see button
		  {
		    d_enabled = not d_enabled;

		  	if(d_listenerFunction)
		  		d_listenerFunction(d_enabled);
		  		
		  	return true;
		  }
		  d_selected = true;
		}
		else
		  d_selected = false;
		  
		return false;
	}

	void Switch::draw(int x, int y)
	{	  
	  if(d_context == 0)
	    return;
	  
	  x += d_x;
	  y += d_y;

    vec4 color;

    if(d_selected)
      color = d_context->hoverColor();

		mat4 modelMatrix(1.0);

    modelMatrix = translate(modelMatrix, vec3(x, y, 0));
	  modelMatrix = scale(modelMatrix, vec3(d_width, d_height, 1.0));
	  d_context->shader().set("in_mat_model", modelMatrix);

	  d_context->shader().set("in_color", color);
	  if(d_enabled)
	    d_context->shader().set("in_texture", d_context->switchPressedTexture(), 0);
	  else
	    d_context->shader().set("in_texture", d_context->switchTexture(), 0);
	  d_context->shader().set("in_text", d_text, 1);
	  d_context->mesh().draw();
	  
	  d_context->shader().set("in_color", vec4(0.0));
	  d_context->shader().set("in_texture", d_context->switchOverlayTexture(), 0);
    d_context->shader().set("in_text", Texture<>::zeroTexture(), 1);
	  d_context->mesh().draw();
	}
	
	bool Switch::enabled() const
	{
	  return d_enabled;
	}
	
	void Switch::enable(bool state)
	{
	  d_enabled = state;
	}

	void Switch::setListener(std::function<void(bool)> const &listenerFunction)
	{
		d_listenerFunction = listenerFunction;
	}

  Component *Switch::v_clone() const
  {
    return new Switch(*this);
  }

}
