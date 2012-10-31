// frame.cpp
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

#include <algorithm>

#include "dim/frame.hpp"

#include "GL/glfw.h"

#include "dim/mesh.hpp"

using namespace std;
using namespace glm;

namespace dim
{	
	Frame::Frame(Texture<GLubyte> const &defaultTexture,
			size_t width, size_t height, Font const &font)
			:
			  d_x(0),
			  d_y(0),
			  d_width(width),
			  d_height(height),
			  d_cam(Camera::flat, d_width, d_height),
			  d_components([](Component *ptr){return ptr->clone();}),
			  d_context(new Context(defaultTexture, font))
	{

	}

	void Frame::add(Component *component)
	{
	  component->setContext(d_context.get());
	  //component.setId(d_components.size());
	  
	  auto itToInsert = lower_bound(d_components.begin(), d_components.end(), component, [](Component *left, Component *right)
	  {
	    return *left < *right;
	  });
	  
		d_components.insert(itToInsert, component);
	}

	void Frame::draw()
	{
	  d_context->shader().use();
	  d_cam.setAtShader(d_context->shader(), "in_mat_view", "in_mat_projection");
	  d_context->shader().set("in_mat_model", glm::mat4(1.0));

	  d_context->mesh().bind();
		for (size_t idx = 0; idx != d_components.size(); ++idx)
		{
			d_components[idx]->draw(d_x, d_y);
		}
		d_context->mesh().unbind();
	}
	
	void Frame::listen(Mouse const &mouse)
	{
		for (size_t idx = d_components.size(); idx--;)
		{
			if(d_components[idx]->listen(d_x, d_y, mouse))
			  break;
		}
	}

  Context &Frame::context()
  {
    return *d_context;
  }
}
