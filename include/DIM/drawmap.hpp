//      drawmap.hpp
//      
//      Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#ifndef DRAWMAP_HPP
#define DRAWMAP_HPP

#include "DIM/drawablewrapper.hpp"
#include "DIM/drawstate.hpp"

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <memory>

namespace dim
{

  class DrawMap
  {
      std::set<DrawState> d_drawStateList;

      std::vector<DrawableWrapper__<Drawable>*> d_drawableWrappers;

      size_t d_gridSize;

    public:
      //typedefs + iterators
      //template <typename RefType>
      //using iterator = typename DrawableWrapper__<RefType>::iterator;
      //template <typename RefType>
      //using const_iterator = typename DrawableWrapper__<RefType>::const_iterator;

      typedef std::pair<size_t, DrawableWrapper__<Drawable>::iterator> IdType;

      typedef base_iterator__<Drawable, DrawMap, IdType> iterator;
      typedef base_iterator__<Drawable const, DrawMap const, IdType> const_iterator;

      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator begin();
      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator end();
      template<typename RefType>
      typename DrawableWrapper__<RefType>::const_iterator begin() const;
      template<typename RefType>
      typename DrawableWrapper__<RefType>::const_iterator end() const;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

      IdType nextId(IdType const &id) const;
      //IdType const &previousId(IdType const &id) const;
      Drawable &getFromId(IdType const &id);
      Drawable const &getFromId(IdType const &id) const;

      //iterators

      enum DrawMode
      {
        normal, shadow,
      };

      explicit DrawMap(size_t gridSize = 64);
      ~DrawMap();

      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator add(bool saved, RefType const &object);

      template<typename RefType>
      void load(std::string const &filename);

      void save();
      void reset();

      void del(DrawMap::iterator object);
      DrawMap::iterator get(float x, float z);

      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator get(float x, float z);

      void draw(DrawMode mode);

      void mark(DrawMap::iterator const &object);

      //void change(Drawable const &object);

    private:
      DrawMap::iterator d_objSelect;
      void add(DrawState const &state);
      DrawMap::iterator find(float x, float z);
  };

  template<typename RefType>
  void DrawMap::load(std::string const &filename)
  {
    if(DrawableWrapper__<RefType>::instance().gridSize() == 0)
      d_drawableWrappers.push_back(new DrawableWrapper__<RefType>(d_gridSize));

    DrawableWrapper__<RefType>::instance().load(filename);

    // Add the drawstates
    for(size_t idx = 0; idx != RefType().drawStates().size(); ++idx)
      add(RefType().drawStates()[idx]);
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawMap::add(bool saved, RefType const &object)
  {
    // Add the object
    if(DrawableWrapper__<RefType>::instance().gridSize() == 0)
      d_drawableWrappers.push_back(new DrawableWrapper__<RefType>(d_gridSize));

    typename DrawableWrapper__<RefType>::iterator iter =
        DrawableWrapper__<RefType>::instance().add(!saved, object);
        
    // Add the drawstates
    for(size_t idx = 0; idx != object.drawStates().size(); ++idx)
      add(object.drawStates()[idx]);

    return iter;
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawMap::get(float x, float z)
  { if(DrawableWrapper__<RefType>::instance().gridSize() == 0)
      return end();

    return DrawableWrapper__<RefType>::instance().find(x, z);
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawMap::begin()
  {
    return DrawableWrapper__<RefType>::instance().begin();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawMap::end()
  {
    return DrawableWrapper__<RefType>::instance().end();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator DrawMap::begin() const
  {
    return DrawableWrapper__<RefType>::instance().begin();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator DrawMap::end() const
  {
    return DrawableWrapper__<RefType>::instance().end();
  }

}

#endif
