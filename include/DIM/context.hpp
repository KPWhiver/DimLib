// context.hpp
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

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <vector>

#include "DIM/mesh.hpp"
#include "DIM/component.hpp"
#include "DIM/font.hpp"
#include "DIM/mouse.hpp"

namespace dim
{
	class Component;

	class Context
	{
		std::vector<Component*> d_components;

		int d_x, d_y;
		size_t d_width;
		size_t d_height;

		static Mesh *s_mesh;
		static bool s_initialized;

		Texture d_butTexture;
		Texture d_butHoverTexture;
		Texture d_butDisableTexture;
		
		Mouse const *d_mouse;

		Font d_font;

	public:
		Context(Mouse const &mouse, Texture const &but, Texture const &butHover, Texture const &butDisable, size_t width, size_t height, Font const &font);

		void add(Component *component);

		void draw() const;

		void setCoords(int x, int y);
		void setSize(size_t width, size_t height);

		int x() const;
		int y() const;

		Mouse const &mouse() const;

		Texture const &buttonTexture() const;
		Texture const &buttonHoverTexture() const;
		Texture const &buttonDisableTexture() const;
		Font &font();

		Mesh const &mesh() const;
	};

}

#endif /* CONTEXT_HPP_ */
