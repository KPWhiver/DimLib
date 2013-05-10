// sprite.cpp
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

#include "dim/gui/sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace dim
{
	Sprite::Sprite(int x, int y, size_t width, size_t height, Texture<GLubyte> const &image)
	:
	    d_x(x),
		  d_y(y),
			d_width(width),
			d_height(height),
			d_image(image),
			d_x1Tex(0),
			d_y1Tex(0),
			d_x2Tex(1),
			d_y2Tex(1),
			d_color(1)
	{
		d_priority = 50;
	}
		
	void Sprite::draw()
	{	  
	  draw(0, 0);
	}
	
	void Sprite::drawWithOffset(int x, int y)
	{	
	  x += d_x;
	  y += d_y;

	  shader().set("in_texture0", d_image, 0);
	  shader().set("in_textureArea", vec4(d_x1Tex, d_y1Tex, d_x2Tex, d_y2Tex));
	  shader().set("in_color", d_color);
	  shader().set("in_channels", d_image.numberOfChannels());

	  mat4 modelMatrix(1.0);
    modelMatrix = translate(modelMatrix, vec3(x, y, 0));
	  modelMatrix = scale(modelMatrix, vec3(d_width, d_height, 1.0));
	  shader().set("in_mat_model", modelMatrix);
	  
	  mesh().draw();
	}

  void Sprite::setLocation(int x, int y)
  {
    d_x = x;
    d_y = y;
  }
    
  void Sprite::setTexture(Texture<GLubyte> const &image)
  {
    d_image = image;
  }
  
  void Sprite::setTextureArea(int x, int y, uint width, uint height)
  {
    d_x1Tex = x / d_image.width();
    d_y1Tex = y / d_image.height();
    d_x2Tex = (x + width) / d_image.width();
    d_y2Tex = (y + height) / d_image.height();
  }
  
  void Sprite::setColor(vec4 const &color)
  {
    d_color = color;
  }
  
	Mesh const &Sprite::mesh() const
	{
		static GLfloat interleaved[24]{0, 0,  0, 0,  0, 1,  0, 1,  1, 0,  1, 0,  1, 1,  1, 1,  1, 0,  1, 0,  0, 1,  0, 1};
		
		static Mesh mesh(interleaved, {{Attribute::vertex, Attribute::vec2}, {Attribute::texCoord, Attribute::vec2}}, 6, Mesh::triangle, Mesh::interleaved);
		
		return mesh;
	}
  
  Shader const &Sprite::shader() const
	{
	  static Shader paletShader("spriteShader", "#version 120\n"
	                            "vertex:\n"
	                            "uniform mat4 in_mat_projection;\n uniform mat4 in_mat_view;\n uniform mat4 in_mat_model;\n"
	                            "uniform vec4 in_textureArea;"
	                            "layout(location = dim_vertex) attribute vec2 in_position;\n layout(location = dim_texCoord) attribute vec2 in_texcoord;\n"
	                            "varying vec2 pass_texcoord;\n"
	                            "void main(){\n" 
	                            "pass_texcoord.x = in_textureArea.x * (!in_texcoord.x) + in_textureArea.z * (in_texcoord.x);\n"
	                            "pass_texcoord.y = in_textureArea.y * (!in_texcoord.y) + in_textureArea.w * (in_texcoord.y);\n"
	                            "gl_Position = in_mat_projection * in_mat_model * in_mat_view * vec4(in_position, vec2(1.0));}\n"
	                            "fragment:\n"
	                            "uniform sampler2D in_texture;\n"
	                            "uniform sampler2D in_text;\n"
	                            "uniform vec4 in_color;\n"
	                            "uniform vec4 in_channels;\n"
	                            "varying vec2 pass_texcoord;\n"
	                            "void main(){
	                            vec4 texColor = texture2D(in_texture, pass_texcoord);
	                            if(in_channels == 1)
	                              texColor = texColor.r * in_color;
	                            else if(in_channels == 2)
	                            {
	                              texColor.rgb = texColor.r * in_color.rgb;
	                              texColor.a = texColor.b * in_color.a;
	                            }
	                            gl_FragColor = texture2D(in_texture, pass_texcoord)\n"
	                            "                           - vec4(vec3(texture2D(in_text, pass_texcoord).r), 0.0) "
	                            "                           + in_color;}\n");

	  return paletShader;
	}
}
