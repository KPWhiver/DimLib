// button.cpp
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

#include "dim/gui/button.hpp"
#include "dim/core/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace glm;
using namespace std;

namespace dim
{

  //Button::Shared::Shared()
  //:
  //    d_menu(0)
  //{
  //}

	Button::Button(int x, int y, size_t width, size_t height, string const &text)
	:
			d_x(x),
			d_y(y),
			d_width(width),
			d_height(height),
			d_strText(text),
			d_selected(false)
	{
		d_priority = 50;
	}
	
	Button::Button()
	:
			d_x(0),
			d_y(0),
			d_width(10),
			d_height(10),
			d_selected(false)
	{
		d_priority = 50;
	}
	
	void Button::setContext(Context *context)
	{
		d_context = context;
	  d_text = Texture<GLubyte>(d_context->font().generateTexture(d_strText, true, d_width, d_height, Filtering::linear));
	  
	  if(d_menu.get() != 0)
	  	d_menu->setContext(context);
	}
	
	void Button::setMenu(Menu *menu)
	{
	  d_menu.reset(menu);
	}
	
	bool Button::listen(int x, int y, glm::ivec2 const &mouse)
	{ 
	  if(d_menu.get() != 0 && d_menu->listen(x, y, mouse))
	    return true;
	  
	  x += d_x;
	  y += d_y;

		if(mouse.x > x && mouse.x < x + static_cast<int>(d_width) && mouse.y > y && mouse.y < y + static_cast<int>(d_height))
		{
			if(true) //TODO: should be on mouse click
		  {
		  	if(d_listenerFunction)
		  		d_listenerFunction();

		  	if(d_menu.get() != 0)
		  	{
		  		if(d_menu->active())
		  			d_menu->deactivate();
		  		else
		  			d_menu->activate(d_x, d_y);
		  	}
		  			
		  	return true;
		  }
		  d_selected = true;
		}
		else
		  d_selected = false;
		  
		return false;
	}

	void Button::draw(int x, int y)
	{	  
	  if(d_context == 0)
	    return;

	  if(d_menu.get() != 0)
	    d_menu->draw(x, y);
	  
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
	  d_context->shader().set("in_texture", d_context->buttonTexture(), 0);
	  d_context->shader().set("in_text", d_text, 1);
	  d_context->mesh().draw();
	  
	  d_context->shader().set("in_color", vec4(0.0));
	  d_context->shader().set("in_texture", d_context->buttonOverlayTexture(), 0);
    d_context->shader().set("in_text", Texture<>::zeroTexture(), 1);
	  d_context->mesh().draw();
	}

	void Button::setListener(std::function<void()> const &listenerFunction)
	{
		d_listenerFunction = listenerFunction;
	}

  Component *Button::v_clone() const
  {
    return new Button(*this);
  }

}
