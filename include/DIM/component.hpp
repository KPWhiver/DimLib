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

#include "DIM/context.hpp"

namespace dim {
	class Context;

	class Component
	{
		protected:
		Context* d_context;

		size_t d_priority;

		public:
		Component();

		virtual bool listen();
		virtual void draw();


		void setPriority(size_t priority);
		size_t priority();
		bool operator<(Component const &component);

		virtual ~Component();

		virtual void setContext(Context *context);

	};

} /* namespace dim */
#endif /* COMPONENT_HPP_ */
