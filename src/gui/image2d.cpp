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

#include "dim/gui/image2d.hpp"
#include "dim/core/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace dim
{
  Texture<> Image2D::defaultTexture()
  {
    static GLubyte data(0);
    static Texture<> texture(&data, Filtering::nearest, NormalizedFormat::R8, 1, 1, false);
    return texture;
  }

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

	  d_context->shader().set("in_texture0", d_image, 0);
	  d_context->shader().set("in_text", defaultTexture(), 1);

	  mat4 modelMatrix(1.0);
    modelMatrix = translate(modelMatrix, vec3(x, y, 0));
	  modelMatrix = scale(modelMatrix, vec3(d_width, d_height, 1.0));
	  d_context->shader().set("in_mat_model", modelMatrix);
	  
	  d_context->mesh().draw();
	}

  void Image2D::setCoor(int x, int y)
  {
    d_x = x;
    d_y = y;
  }

  Component *Image2D::v_clone() const
  {
    return new Image2D(*this);
  }


}
