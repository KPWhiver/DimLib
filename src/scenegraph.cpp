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
  /* setChanged */
  void SceneGraph::setRotation(vec3 const &rot)
  {
    for(DrawNodeBase &drawNode: *this)
      drawNode.setChanged();
      
    GroupNodeBase::setRotation(rot);
  }
  void SceneGraph:;setScaling(vec3 const &scale)
  {
    for(DrawNodeBase &drawNode: *this)
      drawNode.setChanged();
      
    GroupNodeBase::setScaling(scale);
  
  }
  void SceneGraph::setCoor(vec3 const &coor)
  {
    for(DrawNodeBase &drawNode: *this)
      drawNode.setChanged();
      
    GroupNodeBase::setCoor(coor);
  
  }
  
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
        return iterator{CopyPtr<Iterable>(new Iterable(iter, idx, this))};
    }

    return end();
  }

  SceneGraph::iterator SceneGraph::end()
  {
    return iterator{CopyPtr<Iterable>(new Iterable(d_storages.back()->end(), d_storages.size(), this))};
  }

 /* SceneGraph::const_iterator SceneGraph::begin() const
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
  }*/

  SceneGraph::Iterable::Iterable(internal::NodeStorageBase::iterator iter, size_t idx, SceneGraph *container)
  :
      d_iterator(iter),
      d_listIdx(idx),
      d_container(container)
  {
  }

  void SceneGraph::Iterable::increment()
  {
    ++d_iterator;

    if(d_iterator == d_container->d_storages[d_listIdx]->end())
    {
      for(size_t idx = d_listIdx; idx != d_container->d_storages.size(); ++idx)
      {
        d_iterator = d_container->d_storages[idx]->begin();

        if(d_iterator != d_container->d_storages[idx]->end())
        {
          d_listIdx = idx;
          return;
        }
      }

      d_listIdx = d_container->d_storages.size();
    }
  }

  DrawNodeBase &SceneGraph::Iterable::dereference()
  {
    return *d_iterator;
  }

  DrawNodeBase const &SceneGraph::Iterable::dereference() const
  {
    return *d_iterator;
  }

  bool SceneGraph::Iterable::equal(CopyPtr<Iterable> const &other) const
  {
    return d_iterator == other.get()->d_iterator && d_listIdx == other.get()->d_listIdx;
  }

  void SceneGraph::Iterable::erase()
  {
    d_container->d_storages[d_listIdx]->del(d_iterator);
  }

  /* regular functions */

  void SceneGraph::add(ShaderScene const &state, NodeStorageBase *ptr)
  {
    for(auto iter = d_drawStates.find(state); iter != d_drawStates.end() && iter->first == state; ++iter)
    {
      if(iter->second == ptr)
        return;
    }

    d_drawStates.insert(make_pair(state, ptr));
  }

  void SceneGraph::draw(Camera camera)
  {
    for(auto const &element: d_drawStates)
    {
      // TODO optimize optimize optimize
      ShaderScene const &state = element.first;

      state.shader.use();
      
      camera.setAtShader(state.shader, "in_mat_view", "in_mat_projection");

      state.state.mesh().bind();
      if(state.state.culling() == false)
        glDisable(GL_CULL_FACE);

      for(size_t tex = 0; tex != state.state.textures().size(); ++tex)
        state.shader.set(state.state.textures()[tex].second, state.state.textures()[tex].first, tex);

      element.second->draw(state);
      
      state.state.mesh().unbind();
      if(state.state.culling() == false)
        glEnable(GL_CULL_FACE);
    }
  }

  void SceneGraph::del(SceneGraph::iterator object)
  {
    if(object == end())
      return;

    object.iterable()->erase();
  }

  SceneGraph::iterator SceneGraph::find(float x, float z)
  {
    for(size_t idx = 0; idx != d_storages.size(); ++idx)
    {
      auto iter = d_storages[idx]->find(x, z);
      if(iter != d_storages[idx]->end())
        return iterator(CopyPtr<Iterable>(new Iterable(iter, idx, this)));
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
        return iterator(CopyPtr<Iterable>(new Iterable(iter, idx, this)));
    }

    return end();
  }

  size_t SceneGraph::key() const
  {
    return reinterpret_cast<size_t>(this);
  }
}

