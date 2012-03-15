// context.cpp
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

#include "DIM/context.hpp"

#include "GL/glfw.h"

#include "DIM/mesh.hpp"

using namespace std;

namespace dim
{

	//Mesh *Context::s_mesh;
	bool Context::s_initialized = false;

	Context::Context(Mouse const &mouse, Texture const &but, Texture const &butHover, Texture const &butDisable,
			size_t width, size_t height, Font const &font)
			:
				d_x(0),
				d_y(0),
				d_width(width),
				d_height(height),
				d_butTexture(but),
				d_butHoverTexture(butHover),
				d_butDisableTexture(butDisable),
				d_mouse(&mouse),
				d_font(font)
	{

	}
	
	void Context::add(Component *component)
	{
		vector<Component*>::iterator itToInsert = lower_bound(d_components.begin(), d_components.end(), component);
		d_components.insert(itToInsert, component);

		//d_components.push_back(component);
		component->setContext(this);
		
	}

	int Context::x() const
	{
		return d_x;
	}

	int Context::y() const
	{
		return d_y;
	}

	Mouse const &Context::mouse() const
	{
	  return *d_mouse;
	}

	Mesh const &Context::mesh() const
	{
		static GLfloat interleaved[24]{0, 0,  0, 0,  0, 10,  0, 1,  10, 0,  1, 0,  10, 10,  1, 1,  10, 0,  1, 0,  0, 10,  0, 1};
		
		static Mesh mesh(interleaved, "V2T2", 6);
		
		return mesh;
	}

	void Context::draw() const
	{
	  mesh().bind();
		for (size_t idx = 0; idx != d_components.size(); ++idx)
		{
			d_components[idx]->draw();
		}
		mesh().unbind();

		for (size_t idx = d_components.size(); idx--;)
		{
			if(d_components[idx]->listen())
			  break;
		}
	}

	Texture const &Context::buttonTexture() const
	{
		return d_butTexture;
	}

	Texture const &Context::buttonHoverTexture() const
	{
		return d_butHoverTexture;
	}

	Texture const &Context::buttonDisableTexture() const
	{
		return d_butDisableTexture;
	}
	
	Font &Context::font()
	{
	  return d_font;
	}

}
