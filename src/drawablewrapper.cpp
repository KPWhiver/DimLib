// drawablewrapper.cpp
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

#include "dim/drawablewrapper.hpp"

using namespace std;

namespace dim
{
namespace internal
{
  /* drawState */
  Mesh const &DrawState::mesh() const
  {
    return d_mesh;
  }

  Shader const &DrawState::shader() const
  {
    return d_shader;
  }

  bool DrawState::operator==(DrawState const &other)
  {
    return forward_as_tuple(other.d_shader.id(), other.d_mesh.textures()[0].first.id(), other.d_mesh.id()) ==
           forward_as_tuple(other.d_shader.id(), other.d_mesh.textures()[0].first.id(), other.d_mesh.id());
  }

  bool DrawState::operator<(DrawState const &other)
  {
    return forward_as_tuple(other.d_shader.id(), other.d_mesh.textures()[0].first.id(), other.d_mesh.id()) <
           forward_as_tuple(other.d_shader.id(), other.d_mesh.textures()[0].first.id(), other.d_mesh.id());
  }

  /* constructors */
  DrawableWrapper<Drawable>::DrawableWrapper(size_t ownerId)
      :
          d_ownerId(ownerId)
  {
  }  
  
  DrawableWrapper<Drawable>::~DrawableWrapper()
  {
  }
  
  void DrawableWrapper<Drawable>::copy(size_t dest)
  {
    v_copy(dest);
    d_ownerId = dest;
  }
  
  DrawableWrapper<Drawable>* DrawableWrapper<Drawable>::clone() const
  {
    return v_clone();
  }
  
  /* general functions */
  void DrawableWrapper<Drawable>::clear()
  {
    v_clear();
  }

  void DrawableWrapper<Drawable>::draw(DrawState const &state)
  {
    v_draw(state);
  }  
  
  DrawableWrapper<Drawable>::iterator DrawableWrapper<Drawable>::find(float x, float z)
  {
    return v_find(x, z);
  }

  DrawableWrapper<Drawable>::iterator DrawableWrapper<Drawable>::find(DrawState const &state, float x, float z)
  {
    return v_find(state, x, z);
  }

  void DrawableWrapper<Drawable>::del(iterator &object)
  {
    v_del(object);
  }

  /* iterators */
  void DrawableWrapper<Drawable>::increment(IterType *iter) const
  {
    v_increment(iter);
  }

  Drawable &DrawableWrapper<Drawable>::dereference(IterType const &iter)
  {
    return v_dereference(iter);
  }

  Drawable const &DrawableWrapper<Drawable>::dereference(IterType const &iter) const
  {
    return v_dereference(iter);
  }

  bool DrawableWrapper<Drawable>::equal(IterType const &lhs, IterType const &rhs) const
  {
    return v_equal(lhs, rhs);
  }

  DrawableWrapper<Drawable>::iterator DrawableWrapper<Drawable>::begin()
  {
    return v_begin();
  }

  DrawableWrapper<Drawable>::iterator DrawableWrapper<Drawable>::end()
  {
    return v_end();
  }

  DrawableWrapper<Drawable>::const_iterator DrawableWrapper<Drawable>::begin() const
  {
    return v_begin();
  }

  DrawableWrapper<Drawable>::const_iterator DrawableWrapper<Drawable>::end() const
  {
    return v_end();
  }

  /* private functions */
  size_t DrawableWrapper<Drawable>::ownerId() const
  {
    return d_ownerId;
  }

}
}
