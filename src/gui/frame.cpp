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

#include "dim/gui/frame.hpp"


#include "dim/core/mesh.hpp"

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
				d_context(new Context(defaultTexture, font)),
				d_leftEvent(Component::released),
				d_rightEvent(Component::released)
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
	
	void Frame::listen(ivec2 const &mouse, Component::Event leftEvent, Component::Event rightEvent)
	{
		for (size_t idx = d_components.size(); idx--;)
		{
			if(d_components[idx]->listen(d_x, d_y, mouse, leftEvent, rightEvent))
			  break;
		}
	}

	void Frame::listen(ivec2 const &mouse, bool leftPressed, bool rightPressed)
	{
		if(leftPressed)
		{
			if(d_leftEvent == Component::released || d_leftEvent == Component::release)
				d_leftEvent = Component::press;
			else
				d_leftEvent = Component::pressed;
		}
		else
		{
			if(d_leftEvent == Component::pressed || d_leftEvent == Component::press)
				d_leftEvent = Component::release;
			else
				d_leftEvent = Component::released;
		}

		if(rightPressed)
		{
			if(d_rightEvent == Component::released || d_leftEvent == Component::release)
				d_rightEvent = Component::press;
			else
				d_rightEvent = Component::pressed;
		}
		else
		{
			if(d_rightEvent == Component::pressed || d_rightEvent == Component::press)
				d_rightEvent = Component::release;
			else
				d_rightEvent = Component::released;
		}

		listen(mouse, d_leftEvent, d_rightEvent);
	}

  Context &Frame::context()
  {
    return *d_context;
  }
}
