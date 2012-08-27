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

#include "dim/button.hpp"
#include "dim/shader.hpp"

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
	    //Button(0, 0, 10, 10, "")
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
	  d_text = Texture<GLubyte>(d_context->font().generateTexture(d_strText, d_width, d_height));
	  
	  if(d_menu != 0)
	  	d_menu->setContext(context);
	}
	
	void Button::setMenu(Menu *menu)
	{
	  d_menu.reset(menu);
	}
	
	bool Button::listen(int x, int y, dim::Mouse const &mouse)
	{
	  ivec2 mouseC = mouse.coor();
	  
	  x += d_x;
	  y += d_y;
	  

	  if(d_menu != 0 && d_menu->listen(mouseC.x, mouseC.y, mouse))
	    return true;

	  
	  //if(mouseC.x > x && mouseC.x < x + static_cast<int>(d_width) && mouse.y > y && mouse.y < y + static_cast<int>(d_height))
		//  d_context->buttonHoverTexture().send(0, "in_texture0");
		//else
		//  d_context->buttonTexture().send(0, "in_texture0");
	  
		if(mouseC.x > x && mouseC.x < x + static_cast<int>(d_width) && mouseC.y > y && mouseC.y < y + static_cast<int>(d_height))
		{
		  if(mouse.lRelease())
		  {
		  	if(d_listenerFunction)
		  		d_listenerFunction();

		  	if(d_menu != 0)
		  	{
		  		if(d_menu->active())
		  			d_menu->deactivate();
		  		else
		  			d_menu->activate(x, y);
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
	  
	  x += d_x;
	  y += d_y;
	  
	  if(d_menu != 0)
	  {
	    d_menu->draw(x, y);
	  }
	  
		if(d_selected == true)
		  d_context->buttonHoverTexture().send(0, "in_texture0");
		else
		  d_context->buttonTexture().send(0, "in_texture0");

    Shader::in_mat_model() = translate(Shader::in_mat_model(), vec3(x, y, 0));
	  Shader::in_mat_model() = scale(Shader::in_mat_model(), vec3(d_width/10.0, d_height/10.0, 1.0));
	  Shader::active().send(Shader::in_mat_model(), "in_mat_model");
	  
    Shader::active().transformBegin();

	  d_context->mesh().draw();

	  d_text.send(0, "in_texture0");

	  d_context->mesh().draw();

	  Shader::active().transformEnd();
	}

	void Button::setListener(std::function<void()> const &listenerFunction)
	{
		d_listenerFunction = listenerFunction;
	}

}
