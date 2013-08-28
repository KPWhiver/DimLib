// component.cpp
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

#include "dim/gui/component.hpp"

namespace dim {

Component::Component()
:
		d_context(0),
		d_priority(0)//,
		//d_id(0)
{
}

Component::~Component()
{
}

void Component::draw(int x, int y)
{
}

void Component::setContext(Context *context)
{
	d_context = context;
}

bool Component::listen(int x, int y, glm::ivec2 const &mouse, Event leftEvent, Event rightEvent)
{
	return false;
}

void Component::setPriority(size_t priority)
{
	d_priority = priority;
}

size_t Component::priority() const
{
	return d_priority;
}

//void Component::setId(size_t id)
//{
//  d_id = id;
//}

//size_t Component::id() const
//{
//  return d_id;
//}

Component *Component::clone() const
{
  return v_clone();
}

bool Component::operator<(Component const &component)
{
	return d_priority < component.d_priority;
}

} /* namespace dim */
