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

#include "dim/drawablewrapper.hpp"
#include "dim/drawstate.hpp"
#include "dim/cloneptr.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>

namespace dim
{

  class SceneGraph
  {
      std::multimap<DrawState, DrawableWrapper__<Drawable>*> d_drawStateList;

      std::vector<ClonePtr<DrawableWrapper__<Drawable>>> d_drawableWrappers;

      size_t d_gridSize;

    public:
      //typedefs + iterators
      //template <typename RefType>
      //using iterator = typename DrawableWrapper__<RefType>::iterator;
      //template <typename RefType>
      //using const_iterator = typename DrawableWrapper__<RefType>::const_iterator;

    // iterators
      typedef DrawableWrapper__<Drawable>::iterator IdType;

      typedef base_iterator__<Drawable, SceneGraph, IdType> iterator;
      typedef base_iterator__<Drawable const, SceneGraph const, IdType> const_iterator;

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

    // constructors
      enum DrawMode
      {
        normal, shadow,
      };

      explicit SceneGraph(size_t gridSize = 64);
      
      SceneGraph(SceneGraph const &other);
      SceneGraph(SceneGraph &&tmp);
      
      SceneGraph &operator=(SceneGraph const &other);
      SceneGraph &operator=(SceneGraph &&tmp);
      
      ~SceneGraph();


    // regular functions
      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator add(bool saved, RefType *object);

      template<typename RefType>
      void load(std::string const &filename);

      void save();
      void reset();

      void del(SceneGraph::iterator object);
      SceneGraph::iterator get(float x, float z);

      template<typename RefType>
      typename DrawableWrapper__<RefType>::iterator get(float x, float z);

      void draw(DrawMode mode);

      //void mark(SceneGraph::iterator const &object);

      //void change(Drawable const &object);

    private:
      //SceneGraph::iterator d_objSelect;
      void add(DrawState const &state, DrawableWrapper<Drawable>* ptr);
      SceneGraph::iterator find(float x, float z);
  };

  template<typename RefType>
  void SceneGraph::load(std::string const &filename)
  {
    if(not DrawableWrapper__<RefType>::isPresent(this))
      d_drawableWrappers.push_back(new DrawableWrapper__<RefType>(d_gridSize, this));

    /* open the file */
    std::ifstream file(filename().c_str());
    if(file.is_open() == false)
      throw std::runtime_error("Error opening " + filename());
    else
    {
      size_t numItems;
      file >> numItems;
      for(size_t idx = 0; idx != numItems; ++idx)
      {
        RefType ref;
        file >> ref;
        file.ignore();
        add(false, ref);
      }
      file.close();
    }

    // Add the drawstates
    for(size_t idx = 0; idx != RefType().drawState().size(); ++idx)
      add(RefType().drawState()[idx], d_drawableWrappers.back());
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator SceneGraph::add(bool saved, RefType *object)
  {
    DrawableWrapper__<RefType>* ptr = 0;

    // Add the object
    if(not DrawableWrapper__<RefType>::isPresent(this))
    {
      ptr = new DrawableWrapper__<RefType>(d_gridSize, this);
      d_drawableWrappers.push_back(ptr);
    }
    else
    {
      ptr = &DrawableWrapper__<RefType>::get(this);
    }

    typename DrawableWrapper__<RefType>::iterator iter = ptr->add(!saved, object);
        
    // Add the drawstates
    for(size_t idx = 0; idx != object.drawStates().size(); ++idx)
      add(object.drawStates()[idx]);

    return iter;
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator SceneGraph::get(float x, float z)
  { if(DrawableWrapper__<RefType>::instance().gridSize() == 0)
      return end();

    return DrawableWrapper__<RefType>::instance().find(x, z);
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator SceneGraph::begin()
  {
    return DrawableWrapper__<RefType>::instance().begin();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator SceneGraph::end()
  {
    return DrawableWrapper__<RefType>::instance().end();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator SceneGraph::begin() const
  {
    return DrawableWrapper__<RefType>::instance().begin();
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator SceneGraph::end() const
  {
    return DrawableWrapper__<RefType>::instance().end();
  }

}

#endif
