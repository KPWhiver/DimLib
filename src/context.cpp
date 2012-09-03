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

#include "dim/context.hpp"

#include "GL/glfw.h"

using namespace std;

namespace dim
{
	bool Context::s_initialized = false;
	
	Context::Context(Texture<GLubyte> const &but, Texture<GLubyte> const &butHover, Texture<GLubyte> const &butDisable,
			Font const &font, Shader const &shader)
			:
				d_butTexture(but),
				d_butHoverTexture(butHover),
				d_butDisableTexture(butDisable),
				d_font(font),
				d_shader(shader)
	{

	}
	
	Mesh const &Context::mesh() const
	{
		static GLfloat interleaved[24]{0, 0,  0, 0,  0, 10,  0, 1,  10, 0,  1, 0,  10, 10,  1, 1,  10, 0,  1, 0,  0, 10,  0, 1};
		
		static Mesh mesh(interleaved, {{Attribute::vertex, Attribute::vec2}, {Attribute::texCoord, Attribute::vec2}}, 6, Mesh::triangle, Mesh::interleaved);
		
		return mesh;
	}

	Texture<GLubyte> const &Context::buttonTexture() const
	{
		return d_butTexture;
	}

	Texture<GLubyte> const &Context::buttonHoverTexture() const
	{
		return d_butHoverTexture;
	}

	Texture<GLubyte> const &Context::buttonDisableTexture() const
	{
		return d_butDisableTexture;
	}
	
	Font &Context::font()
	{
	  return d_font;
	}

	Shader const &Context::shader() const
	{
	  return d_shader;
	}

}
