// image2d.cpp
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

#include "dim/image2d.hpp"
#include "dim/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace dim
{
	Image2D::Image2D(int x, int y, size_t width, size_t height, Texture<GLubyte> const &image)
	:
	    d_x(x),
		  d_y(y),
			d_width(width),
			d_height(height),
			d_image(image)
	{
		d_priority = 50;
	}
		
	void Image2D::draw(int x, int y)
	{	  
	  if(d_context == 0)
	    return;
	
	  x += d_x;
	  y += d_y;
	  
	  d_image.setAtShader(d_context->shader(), "in_texture0", 0);

    Shader::modelMatrix() = translate(Shader::modelMatrix(), vec3(x, y, 0));
	  Shader::modelMatrix() = scale(Shader::modelMatrix(), vec3(d_width/10.0, d_height/10.0, 1.0));
	  d_context->shader().set("in_mat_model", Shader::modelMatrix());
	  
	  d_context->shader().transformBegin();

	  d_context->mesh().draw();

	  d_context->shader().transformEnd();
	}

  void Image2D::setCoor(int x, int y)
  {
    d_x = x;
    d_y = y;
  }

}
