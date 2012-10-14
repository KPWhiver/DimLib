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
#include "dim/scene.hpp"
#include "dim/ptrvector.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>

namespace dim
{
  struct ShaderScene
  {
    Shader shader;
    DrawState state;

    bool operator==(ShaderScene const &other) const
    {
      return shader.id() == other.shader.id() && state == other.state;
    }

    bool operator<(ShaderScene const &other) const
    {
      if(shader.id() < other.shader.id())
        return true;
      if(not shader.id() == other.shader.id())
        return false;

      if(state < other.state)
        return true;

      return false;
    }
  };

  class SceneGraph
  {
      std::multimap<ShaderScene, internal::NodeStorageBase*> d_drawStates;

      PtrVector<internal::NodeStorageBase> d_storages;

      size_t d_gridSize;//

    public:
      //typedefs + iterators
      template <typename RefType>
      using iterator = typename internal::NodeGrid<RefType>::iterator;//
      template <typename RefType>
      using const_iterator = typename internal::NodeGrid<RefType>::const_iterator;//

    // iterators
      typedef std::pair<internal::NodeStorageBase::iterator, size_t> IterType;

      typedef internal::IteratorBase<Drawable, SceneGraph, IterType> iterator;
      typedef internal::IteratorBase<Drawable const, SceneGraph const, IterType> const_iterator;

      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator begin();//
      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator end();//
      template<typename RefType>
      typename internal::NodeGrid<RefType>::const_iterator begin() const;//
      template<typename RefType>
      typename internal::NodeGrid<RefType>::const_iterator end() const;//

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

      void increment(IterType *iter) const;
      DrawNodeBase &dereference(IterType const &iter);
      DrawNodeBase const &dereference(IterType const &iter) const;
      bool equal(IterType const &lhs, IterType const &rhs) const;

    // constructors
      explicit SceneGraph(size_t gridSize = 64);
      
      SceneGraph(SceneGraph const &other);
      SceneGraph(SceneGraph &&tmp);
      
      SceneGraph &operator=(SceneGraph const &other);
      SceneGraph &operator=(SceneGraph &&tmp);

    // regular functions
      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator add(bool saved, RefType *object);

      template<typename RefType>
      void load(std::string const &filename);

      template<typename RefType>
      void save(std::string const &filename);
      void clear();

      void del(SceneGraph::iterator object);
      SceneGraph::iterator get(float x, float z);

      SceneGraph::iterator get(ShaderScene const &state, float x, float z);

      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator get(float x, float z);

      void draw();

    private:
      void add(ShaderScene const &state, internal::NodeStorageBase* ptr);
      SceneGraph::iterator find(float x, float z);

      size_t key() const;
  };

  // TODO fix
  template<typename RefType>
  void SceneGraph::save(std::string const &filename)
  {
    std::ofstream file(filename.c_str());

    if(file.is_open() == false)
    {
      file.close();
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + filename);
    }

    file << count() << '\n';
    file.precision(0);
    file.setf(std::fstream::fixed);

    for(auto node = begin<RefType>(); node != end<RefType>(); ++node)
      file << *node;

    file.close();
  }

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
  typename internal::NodeGrid<RefType>::iterator SceneGraph::add(bool saved, RefType *object)
  {
    internal::NodeGrid<RefType>* ptr = internal::NodeGrid<RefType>::get(key());

    // Add the object
    if(ptr == 0)
    {
      ptr = new internal::NodeGrid<RefType>(d_gridSize, key());
      d_storages.push_back(ptr);
    }

    typename internal::NodeGrid<RefType>::iterator iter = ptr->add(!saved, object);
        
    // Add the drawstate
    for(size_t idx = 0; idx != object->scene().size(); ++idx)
      add({object->shader(), object->scene()[idx]}, ptr);

    return iter;
  }

  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph::get(float x, float z)
  {
    auto ptr = internal::NodeGrid<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->find(x, z);
  }

  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph::begin()
  {
    auto ptr = internal::NodeGrid<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->begin();
  }

  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph::end()
  {
    auto ptr = internal::NodeGrid<RefType>::get(key());

    if(ptr == 0)
    {
      ptr = new internal::NodeGrid<RefType>(d_gridSize, key());
      d_storages.push_back(ptr);
    }

    return ptr->end();
  }

  template<typename RefType>
  typename internal::NodeGrid<RefType>::const_iterator SceneGraph::begin() const
  {
    auto ptr = internal::NodeGrid<RefType>::get(key());

    if(ptr == 0)
      return end<RefType>();

    return ptr->begin();
  }

  template<typename RefType>
  typename internal::NodeGrid<RefType>::const_iterator SceneGraph::end() const
  {
    auto ptr = internal::NodeGrid<RefType>::get(key());

    if(ptr == 0)
    {
      ptr = new internal::NodeGrid<RefType>(d_gridSize, key());
      d_storages.push_back(ptr);
    }

    return ptr->end();
  }

}

#endif
