// textsprite.cpp
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

#include "dim/gui/textsprite.hpp"
#include "dim/gui/sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

namespace dim
{
	TextSprite::TextSprite(int x, int y, uint width, uint height)
	:
	    d_x(x),
		  d_y(y),
			d_width(width),
			d_height(height),
			d_color(1)
	{
	}
		
	void TextSprite::draw(Font const &font, string const &text)
	{	  
	  drawWithOffset(0, 0, font, text);
	}
	
	void TextSprite::drawWithOffset(int x, int y, Font const &font, string const &text)
	{		    
	  x += d_x;
	  y += d_y;
	  
	  Shader const &shader = Sprite::shader();

	  shader.set("in_texture0", font.map(), 0);
    shader.set("in_color", d_color);
    shader.set("in_channels", 1);
	  uint height = font.height();
	  
	  for(unsigned char ch : text)
	  {
	    ivec2 origin = font.origin(ch);
	    uint advance = font.advance(ch);
	    
	    shader.set("int_textureArea", vec4(origin.x, origin.y, origin.x + advance, origin.y + height));

	    mat4 modelMatrix(1.0);
      modelMatrix = translate(modelMatrix, vec3(x, y, 0));
	    modelMatrix = scale(modelMatrix, vec3(d_width, d_height, 1.0));
	    shader.set("in_mat_model", modelMatrix);
	    
	    Sprite::mesh().draw();
	  }
	}

  void TextSprite::setLocation(int x, int y)
  {
    d_x = x;
    d_y = y;
  }

  void TextSprite::setColor(vec4 const &color)
  {
    d_color = color;
  }
}
