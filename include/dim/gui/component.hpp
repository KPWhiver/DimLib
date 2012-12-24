// component.hpp
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

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "dim/gui/context.hpp"
#include "dim/core/mouse.hpp"

namespace dim 
{
	class Component
	{
		protected:
		  Context *d_context;

		  size_t d_priority;
		  
		  //size_t d_id;

		public:
		  Component();

		  virtual bool listen(int x, int y, dim::Mouse const &mouse);
		  virtual void draw(int x, int y);


		  void setPriority(size_t priority);
		  size_t priority() const;
		  bool operator<(Component const &component);
		  
		  //void setId(size_t id);
		  //size_t id() const;

		  virtual ~Component();

		  virtual void setContext(Context *context);
		  
		  Component *clone() const;

		private:
		  virtual Component *v_clone() const = 0;

		  
	};

} /* namespace dim */
#endif /* COMPONENT_HPP_ */
