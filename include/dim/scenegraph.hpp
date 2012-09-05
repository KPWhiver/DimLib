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
      std::multimap<DrawState, internal::DrawableWrapper<Drawable>*> d_drawStateList;

      std::vector<ClonePtr<internal::DrawableWrapper<Drawable>>> d_drawableWrappers;

      size_t d_gridSize;

    public:
      //typedefs + iterators
      //template <typename RefType>
      //using iterator = typename internal::DrawableWrapper<RefType>::iterator;
      //template <typename RefType>
      //using const_iterator = typename internal::DrawableWrapper<RefType>::const_iterator;

    // iterators
      typedef std::pair<internal::DrawableWrapper<Drawable>::iterator, size_t> IterType;

      typedef internal::IteratorBase<Drawable, SceneGraph, IterType> iterator;
      typedef internal::IteratorBase<Drawable const, SceneGraph const, IterType> const_iterator;

      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::iterator begin();
      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::iterator end();
      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::const_iterator begin() const;
      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::const_iterator end() const;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

      IterType increment(IterType const &iter) const;
      //IdType const &decrement(IdType const &id) const;
      Drawable &dereference(IterType const &iter);
      Drawable const &dereference(IterType const &iter) const;

    // constructors
      explicit SceneGraph(size_t gridSize = 64);
      
      SceneGraph(SceneGraph const &other);
      SceneGraph(SceneGraph &&tmp);
      
      SceneGraph &operator=(SceneGraph const &other);
      SceneGraph &operator=(SceneGraph &&tmp);
      
      ~SceneGraph();


    // regular functions
      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::iterator add(bool saved, RefType *object);

      template<typename RefType>
      void load(std::string const &filename);

      void save(std::string const &filename);
      void clear();

      void del(SceneGraph::iterator object);
      SceneGraph::iterator get(float x, float z);

      SceneGraph::iterator get(DrawState const &state, float x, float z);

      template<typename RefType>
      typename internal::DrawableWrapper<RefType>::iterator get(float x, float z);

      void draw();

      //void change(Drawable const &object);

    private:
      //SceneGraph::iterator d_objSelect;
      void add(DrawState const &state, internal::DrawableWrapper<Drawable>* ptr);
      SceneGraph::iterator find(float x, float z);

      size_t key() const;
  };

  template<typename RefType>
  void SceneGraph::load(std::string const &filename)
  {
    // open the file
    std::ifstream file(filename.c_str());
    if(not file.is_open())
    {
      log(__FILE__, __LINE__, LogType::error, "Failed to open " + filename);
      return;
    }
    else
    {
      //TODO change to till EOF
      size_t numItems;
      file >> numItems;
      for(size_t idx = 0; idx != numItems; ++idx)
      {
        RefType ref = new RefType;
        file >> *ref;
        //file.ignore();

        //TODO room for optimization
        add(true, ref);
      }
      file.close();
    }
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::iterator SceneGraph::add(bool saved, RefType *object)
  {
    internal::DrawableWrapper<RefType>* ptr = internal::DrawableWrapper<RefType>::get(key());

    // Add the object
    if(ptr == 0)
    {
      ptr = new internal::DrawableWrapper<RefType>(d_gridSize, key());
      d_drawableWrappers.push_back(ptr);
    }

    typename internal::DrawableWrapper<RefType>::iterator iter = ptr->add(!saved, object);
        
    // Add the drawstate
    add(object->drawState(), ptr);

    return iter;
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::iterator SceneGraph::get(float x, float z)
  {
    auto ptr = internal::DrawableWrapper<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->find(x, z);
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::iterator SceneGraph::begin()
  {
    auto ptr = internal::DrawableWrapper<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->begin();
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::iterator SceneGraph::end()
  {
    auto ptr = internal::DrawableWrapper<RefType>::get(key());

    if(ptr == 0)
    {
      ptr = new internal::DrawableWrapper<RefType>(d_gridSize, key());
      d_drawableWrappers.push_back(ptr);
    }

    return ptr->end();
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::const_iterator SceneGraph::begin() const
  {
    auto ptr = internal::DrawableWrapper<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->begin();
  }

  template<typename RefType>
  typename internal::DrawableWrapper<RefType>::const_iterator SceneGraph::end() const
  {
    auto ptr = internal::DrawableWrapper<RefType>::get(key());

    if(ptr == 0)
    {
      ptr = new internal::DrawableWrapper<RefType>(d_gridSize, key());
      d_drawableWrappers.push_back(ptr);
    }

    return ptr->end();
  }

}

#endif
