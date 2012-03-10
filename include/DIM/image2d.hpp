// image2d.hpp
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

#ifndef IMAGE2D_HPP
#define IMAGE2D_HPP

#include "DIM/texture.hpp"
#include "DIM/component.hpp"

#include <functional>

namespace dim
{
	class Image2D : public Component
	{
		int d_x, d_y;
		size_t d_width, d_height;
		Texture d_image;

  public:
    Image2D(int x, int y, size_t width, size_t height, Texture const &image);
    void draw();
    void setCoor(int x, int y);

	};

}

#endif /* IMAGE2D_HPP */
