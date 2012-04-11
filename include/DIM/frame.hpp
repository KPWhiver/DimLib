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

#include "DIM/component.hpp"
#include "DIM/mouse.hpp"
#include <memory>

namespace dim
{
	class Frame
	{
	  int d_x, d_y;
		size_t d_width;
		size_t d_height;
		
		std::vector<std::shared_ptr<Component>> d_components;

    std::shared_ptr<Context> d_context;
    
	public:
		Frame(Texture const &but, Texture const &butHover, Texture const &butDisable, size_t width, size_t height, Font const &font);

    //template <typename RefType>
		void add(Component *component);
		void remove(Component *component);

    //template <typename RefType>
		//void update(RefType const &component);

    void listen(dim::Mouse const &mouse);
		void draw();

		void setCoords(int x, int y);
		void setSize(size_t width, size_t height);

		//int x() const;
		//int y() const;

		//Mouse const &mouse() const;

		//Texture const &buttonTexture() const;
		//Texture const &buttonHoverTexture() const;
		//Texture const &buttonDisableTexture() const;
		//Font &font();

		//Mesh const &mesh() const;
	};

  /*template <typename RefType>
	void Frame::add(RefType &component)
	{
	  component.setContext(d_context);
	  component.setId(d_components.size());
	  
	  auto itToInsert = lower_bound(d_components.begin(), d_components.end(), component);
		d_components.insert(itToInsert, new RefType(component));
	}
	
	template <typename RefType>
	void Frame::update(RefType const &component)
	{
    for(size_t idx = 0; idx != d_components.size(); ++idx)
    {
      if(d_components[idx]->id() == component.id())
      {
        delete d_components[idx];
        d_components[idx] = new RefType(component);
        break; 
      }
    }
	}*/
}
#endif /* CONTEXT_HPP_ */
