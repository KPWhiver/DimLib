//      drawmap.cpp
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

#include <fstream>

#include "dim/scenegraph.hpp"
#include "dim/shader.hpp"

#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace glm;

namespace dim
{
  using namespace internal;
  /* constructors */
  
  SceneGraph::SceneGraph(size_t gridSize)
      :
          d_storages([](NodeStorageBase* ptr){return ptr->clone();}),
          d_gridSize(gridSize)
  {
    // make sure there's at least one NodeGrid
    auto ptr = new NodeGrid<DefaultDrawNode>(d_gridSize, key());
    d_storages.push_back(ptr);
  }

  SceneGraph::SceneGraph(SceneGraph const &other)
  :
      d_drawStates(other.d_drawStates),
      d_storages(other.d_storages),
      d_gridSize(other.d_gridSize)
  {
    for(auto const &element: d_storages)
      element->copy(key());
  }

  SceneGraph::SceneGraph(SceneGraph &&tmp)
  :
      d_drawStates(move(tmp.d_drawStates)),
      d_storages(move(tmp.d_storages)),
      d_gridSize(move(tmp.d_gridSize))
  {
    for(auto const &element: d_storages)
      element->copy(key());
  }

  SceneGraph &SceneGraph::operator=(SceneGraph const &other)
  {
    d_drawStates = other.d_drawStates;
    d_storages = other.d_storages;
    d_gridSize = other.d_gridSize;

    for(auto const &element: d_storages)
      element->copy(key());

    return *this;
  }

  SceneGraph &SceneGraph::operator=(SceneGraph &&tmp)
  {
    d_drawStates = move(tmp.d_drawStates);
    d_storages = move(tmp.d_storages);
    d_gridSize = move(tmp.d_gridSize);

    for(auto const &element: d_storages)
      element->copy(key());

    return *this;
  }

  /* iterators */

  SceneGraph::iterator SceneGraph::begin()
  {
    for(size_t idx = 0; idx != d_storages.size(); ++idx)
    {
      NodeStorageBase::iterator iter = d_storages[idx]->begin();

      if(iter != d_storages[idx]->end())
        return iterator(IterType(iter, idx), this);
    }

    return end();
  }

  SceneGraph::iterator SceneGraph::end()
  {
    return iterator(IterType(d_storages.back()->end(), d_storages.size()), this);
  }

  SceneGraph::const_iterator SceneGraph::begin() const
  {
    for(size_t idx = 0; idx != d_storages.size(); ++idx)
    {
      NodeStorageBase::iterator iter = d_storages[idx]->begin();

      if(iter != d_storages[idx]->end())
        return const_iterator(make_pair(iter, idx), this);
    }

    return end();
  }

  SceneGraph::const_iterator SceneGraph::end() const
  {
    return const_iterator(IterType(d_storages.back()->end(), d_storages.size()), this);
  }

  void SceneGraph::increment(IterType *iter) const
  {
    NodeStorageBase::iterator iterate = iter->first;
    ++iterate;

    if(iterate == d_storages[iter->second]->end())
    {
      for(size_t idx = iter->second; idx != d_storages.size(); ++idx)
      {
        iterate = d_storages[idx]->begin();

        if(iterate != d_storages[idx]->end())
          return make_pair(iterate, idx);
      }

      *iter = make_pair(iterate, d_storages.size());
    }
    else
      *iter = make_pair(iterate, iter->second);
  }

  DrawNodeBase &SceneGraph::dereference(IterType const &iter)
  {
    return *iter.first;
  }

  DrawNodeBase const &SceneGraph::dereference(IterType const &iter) const
  {
    return *iter.first;
  }

  bool SceneGraph::equal(IterType const &lhs, IterType const &rhs) const
  {
    return lhs == rhs;
  }

  /* regular functions */

  void SceneGraph::add(ShaderScene const &state, NodeStorageBase *ptr)
  {
    for(auto iter = d_drawStates.find(state); iter->first == state; ++iter)
    {
      if(iter->second == ptr)
        return;
    }

    d_drawStates.insert(make_pair(state, ptr));
  }

  void SceneGraph::draw()
  {
    for(auto const &element: d_drawStates)
    {
      // TODO optimize optimize optimize
      ShaderScene const &state = element.first;

      state.shader().use();
      
      state.mesh().bind();
      if(state.culling() == false)
        glDisable(GL_CULL_FACE);

      for(size_t tex = 0; tex != state.textures().size(); ++tex)
        state.shader().set(state.mesh().textures()[tex].second, state.textures()[tex].first, tex);

      element.second->draw(state);
      
      state.mesh().unbind();
      if(state.culling() == false)
        glEnable(GL_CULL_FACE);
    }
  }

  void SceneGraph::del(SceneGraph::iterator object)
  {
    if(object == end())
      return;

    d_storages[object.iter().second]->del(object.iter().first);
  }

  SceneGraph::iterator SceneGraph::find(float x, float z)
  {
    for(size_t idx = 0; idx != d_storages.size(); ++idx)
    {
      auto iter = d_storages[idx]->find(x, z);
      if(iter != d_storages[idx]->end())
        return iterator(make_pair(iter, idx), this);
    }

    return end();
  }

  void SceneGraph::clear()
  {
    for(auto &element: d_storages)
      element->clear();
  }

  SceneGraph::iterator SceneGraph::get(float x, float z)
  {
    return find(x, z);
  }

  SceneGraph::iterator SceneGraph::get(ShaderScene const &state, float x, float z)
  {
    size_t idx = 0;

    for(auto drawState = d_drawStates.find(state); drawState->first == state; ++drawState, ++idx)
    {
      auto iter = drawState->second->find(state, x, z);
      if(iter != drawState->second->end())
        return iterator(IterType(iter, idx), this);
    }

    return end();
  }

  size_t SceneGraph::key() const
  {
    return reinterpret_cast<size_t>(this);
  }
}

