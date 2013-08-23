// frame.hpp
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

#ifndef FRAME_HPP
#define FRAME_HPP

#include <vector>
#include <memory>

#include "dim/gui/component.hpp"
#include "dim/util/ptrvector.hpp"
#include "dim/core/camera.hpp"
#include "dim/util/copyptr.hpp"
#include <memory>

namespace dim
{
	class Frame
	{
	  int d_x, d_y;
		size_t d_width;
		size_t d_height;

		Camera d_cam;

		dim::PtrVector<Component> d_components;

    std::shared_ptr<Context> d_context;
    
	public:
		Frame(Texture<> const &defaultTexture, size_t width, size_t height, Font const &font);

		void add(Component *component);
		void remove(Component *component);

		void listen(glm::ivec2 const &mouse, Component::Event event);
		void draw();

		void setCoords(int x, int y);
		void setSize(size_t width, size_t height);

    Context &context();
	};
}
#endif /* CONTEXT_HPP_ */
