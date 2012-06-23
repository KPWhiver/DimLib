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

#include "DIM/drawablewrapper.hpp"

namespace dim
{
  /* constructors */
  DrawableWrapper__<Drawable>::DrawableWrapper__(size_t gridSize, size_t ownerId)
      :
          d_changed(false),
          d_gridSize(gridSize),
          d_ownerId(ownerId)
  {
  }  
  
  DrawableWrapper__<Drawable>::~DrawableWrapper__()
  {
    v_remove(d_ownerId);
  }
  
  /* static access */
  
  void DrawableWrapper__<Drawable>::copy(size_t dest) const
  {
    v_copy(ownerId, dest);
  }
  
  void DrawableWrapper__<Drawable>::move(size_t dest) const
  {
    v_move(ownerId, dest);
  }
  
  DrawableWrapper__<Drawable>* DrawableWrapper__<Drawable>::clone() const
  {
    return v_clone();
  }
  
  /* general functions */
  void DrawableWrapper__<Drawable>::save()
  {
    v_save();
  }

  void DrawableWrapper__<Drawable>::reset()
  {
    v_reset();
  }

  void DrawableWrapper__<Drawable>::draw(DrawState const &state, iterator &object)
  {
    v_draw(state, object);
  }  
  
  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<Drawable>::find(float x, float z)
  {
    return v_find(x, z);
  }

  void DrawableWrapper__<Drawable>::del(iterator &object)
  {
    v_del(object);
  }

  /* iterators */
  DrawableWrapper__<Drawable>::IdType DrawableWrapper__<Drawable>::nextId(IdType const &id) const
  {
    return v_nextId(id);
  }

  Drawable &DrawableWrapper__<Drawable>::getFromId(IdType const &id)
  {
    return v_getFromId(id);
  }

  Drawable const &DrawableWrapper__<Drawable>::getFromId(IdType const &id) const
  {
    return v_getFromId(id);
  }

  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<Drawable>::begin()
  {
    return v_begin();
  }

  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<Drawable>::end()
  {
    return v_end();
  }

  DrawableWrapper__<Drawable>::const_iterator DrawableWrapper__<Drawable>::begin() const
  {
    return v_begin();
  }

  DrawableWrapper__<Drawable>::const_iterator DrawableWrapper__<Drawable>::end() const
  {
    return v_end();
  }

  /* private functions */
  size_t DrawableWrapper__<Drawable>::gridSize() const
  {
    return d_gridSize;
  }
  
  size_t DrawableWrapper__<Drawable>::ownerId() const
  {
    return d_ownerId;
  }

  std::string const &DrawableWrapper__<Drawable>::filename() const
  {
    return d_filename;
  }

  void DrawableWrapper__<Drawable>::setFilename(std::string const &filename)
  {
    d_filename = filename;
  }

  bool DrawableWrapper__<Drawable>::changed() const
  {
    return d_changed;
  }

  void DrawableWrapper__<Drawable>::setChanged(bool changed)
  {
    d_changed = changed;
  }
}
