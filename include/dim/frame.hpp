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

#include "dim/component.hpp"
#include "dim/mouse.hpp"
#include "dim/ptrvector.hpp"
#include "dim/camera.hpp"
#include "dim/copyptr.hpp"
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

    void listen(dim::Mouse const &mouse);
		void draw();

		void setCoords(int x, int y);
		void setSize(size_t width, size_t height);

		void setButtonTexture(Texture<> const &button);
		void setButtonOverlayTexture(Texture<> const &overlay);
		void setMenuTextures(Texture<> const &menuTop, Texture<> const &menuMiddle, Texture<> const &menuBottom);
		void setMenuOverlayTextures(Texture<> const &overlayTop, Texture<> const &overlayMiddle, Texture<> const &overlayBottom, Texture<> const &overlaySubmenu);

		void setHoverColor(glm::vec4 const &color);
		void setDisabledColor(glm::vec4 const &color);

		//int x() const;
		//int y() const;

		//Mouse const &mouse() const;

		//Texture const &buttonTexture() const;
		//Texture const &buttonHoverTexture() const;
		//Texture const &buttonDisableTexture() const;
		//Font &font();

		//Mesh const &mesh() const;
	};
}
#endif /* CONTEXT_HPP_ */
