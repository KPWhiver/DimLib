// sprite.hpp
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

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "dim/core/texture.hpp"
#include "dim/core/shader.hpp"
#include "dim/core/mesh.hpp"

namespace dim
{
	class Sprite
	{
	  int d_x;
	  int d_y;
	  		
		uint d_width;
		uint d_height;
		Texture<> d_image;
		
		int d_x1Tex;
		int d_y1Tex;
		int d_x2Tex;
		int d_y2Tex;
		
		glm::vec4 d_color;

  public:
    Sprite(int x, int y, uint width, uint height, Texture<GLubyte> const &image);
    void draw();
    void drawWithOffset(int x, int y);
    void setLocation(int x, int y);
    
    void setTexture(Texture<GLubyte> const &image);
    void setTextureArea(int x, int y, uint width, uint height);
    
    void setColor(glm::vec4 const &color);
    
    bool inBounds(int x, int y) const;

    static Shader const &shader();
    static Mesh const &mesh();
    
    virtual ~Sprite(){};
	};

}

#endif /* SPRITE_HPP */
