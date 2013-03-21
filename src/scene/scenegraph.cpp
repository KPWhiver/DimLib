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

#include "dim/scene/scenegraph.hpp"
#include "dim/core/shader.hpp"
#include "btBulletWorldImporter.h"

#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace glm;

namespace dim
{
  Scene SceneGraph::s_defaultScene;

  using namespace internal;
  /* setChanged */
  void SceneGraph::setOrientation(quat const &orient)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();
      
    NodeBase::setOrientation(orient);
  }
  void SceneGraph::setScaling(vec3 const &scale)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();
      
    NodeBase::setScaling(scale);
  
  }
  void SceneGraph::setLocation(vec3 const &coor)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();
      
    NodeBase::setLocation(coor);
  
  }
  
  /* constructors */
  
  SceneGraph::SceneGraph(size_t numOfRenderModes, size_t gridSize)
      :
          d_storages([](NodeStorageBase* ptr){return ptr->clone();}),
          d_gridSize(gridSize),
          d_numOfRenderModes(numOfRenderModes),
          d_dispatcher(&d_collisionConfiguration),
          d_dynamicsWorld(&d_dispatcher, &d_broadphase, &d_solver, &d_collisionConfiguration)
  {
    // make sure there's at least one NodeGrid
    auto ptr = new NodeGrid<DefaultNode>(d_gridSize, key(), d_numOfRenderModes);
    d_storages.push_back(ptr);

    // bullet
    d_dynamicsWorld.setGravity(btVector3(0, -10, 0));
    d_dynamicsWorld.getDispatchInfo().m_allowedCcdPenetration=0.0001f;
    d_dynamicsWorld.getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

  }

  SceneGraph::SceneGraph(SceneGraph const &other)
  :
      d_drawStates(other.d_drawStates),
      d_storages(other.d_storages),
      d_gridSize(other.d_gridSize),
      d_numOfRenderModes(other.d_numOfRenderModes),
      d_collisionConfiguration(other.d_collisionConfiguration),
      d_dispatcher(other.d_dispatcher),
      d_solver(other.d_solver),
      d_dynamicsWorld(other.d_dynamicsWorld)
  {
    for(auto const &element: d_storages)
      element->copy(key());

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);
  }

  SceneGraph::SceneGraph(SceneGraph &&tmp)
  :
      d_drawStates(move(tmp.d_drawStates)),
      d_storages(move(tmp.d_storages)),
      d_gridSize(move(tmp.d_gridSize)),
      d_numOfRenderModes(tmp.d_numOfRenderModes),
      d_collisionConfiguration(move(tmp.d_collisionConfiguration)),
      d_dispatcher(move(tmp.d_dispatcher)),
      d_solver(move(tmp.d_solver)),
      d_dynamicsWorld(move(tmp.d_dynamicsWorld))
  {
    for(auto const &element: d_storages)
      element->copy(key());

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);
  }

  SceneGraph &SceneGraph::operator=(SceneGraph const &other)
  {
    d_drawStates = other.d_drawStates;
    d_storages = other.d_storages;
    d_gridSize = other.d_gridSize;

    for(auto const &element: d_storages)
      element->copy(key());

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);

    return *this;
  }

  SceneGraph &SceneGraph::operator=(SceneGraph &&tmp)
  {
    d_drawStates = move(tmp.d_drawStates);
    d_storages = move(tmp.d_storages);
    d_gridSize = move(tmp.d_gridSize);

    for(auto const &element: d_storages)
      element->copy(key());

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);

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

  NodeBase &SceneGraph::Iterable::dereference()
  {
    return *d_iterator;
  }

  NodeBase const &SceneGraph::Iterable::dereference() const
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

  //btDiscreteDynamicsWorld *SceneGraph::physicsWorld()
  //{
  //  return &d_dynamicsWorld;
  //}

  void SceneGraph::addRigidBody(btRigidBody *rigidBody)
  {
    if(rigidBody != 0)
      d_dynamicsWorld.addRigidBody(rigidBody);
  }

  void SceneGraph::addAction(btActionInterface *action)
  {
    if(action != 0)
      d_dynamicsWorld.addAction(action);
  }

  void SceneGraph::addGhost(btGhostObject *ghost)
  {
    if(ghost != 0)
      d_dynamicsWorld.addCollisionObject(ghost, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
  }

  void SceneGraph::addBulletFile(std::string const &filename)
  {
    static btBulletWorldImporter fileLoader(&d_dynamicsWorld);
    fileLoader.loadFile(filename.c_str());
  }

  void SceneGraph::add(ShaderScene const &state, NodeStorageBase *ptr)
  {
    auto range = d_drawStates.equal_range(state);

    for(auto iter = range.first; iter != range.second; ++iter)
    {
      if(iter->second == ptr)
        return;
    }

    d_drawStates.insert(make_pair(state, ptr));
  }

  void SceneGraph::physicsStep(float time)
  {
    uint substeps = std::max(1.0f, (1 / 60) / time) + 2;
    d_dynamicsWorld.stepSimulation(time, substeps);
  }

  void SceneGraph::draw(Camera camera, size_t renderMode)
  {
    for(auto const &element: d_drawStates)
    {
      //std::cerr << "SceneGraph::draw()\n";
      // TODO optimize optimize optimize
      ShaderScene const &state = element.first;

      state.shader(renderMode).use();
      
      camera.setAtShader(state.shader(renderMode), "in_mat_view", "in_mat_projection");

      state.state().mesh().bind();
      if(state.state().culling() == false)
        glDisable(GL_CULL_FACE);

      for(size_t tex = 0; tex != state.state().textures().size(); ++tex)
        state.shader(renderMode).set(state.state().textures()[tex].second, state.state().textures()[tex].first, tex);

      element.second->draw(state, renderMode);
      
      state.state().mesh().unbind();
      if(state.state().culling() == false)
        glEnable(GL_CULL_FACE);
    }

    //std::cerr << "\n\n";
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

  void SceneGraph::updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to)
  {
    if(static_cast<size_t>(from.x / d_gridSize) == static_cast<size_t>(to.x / d_gridSize))
    {
      if(static_cast<size_t>(from.z / d_gridSize) == static_cast<size_t>(to.z / d_gridSize))
        return;
    }

    for(auto &storage : d_storages)
    {
      if(storage->updateNode(node, from, to))
        return;
    }
  }

  SceneGraph::iterator SceneGraph::get(float x, float z)
  {
    return find(x, z);
  }

  SceneGraph::iterator SceneGraph::get(NodeBase *node)
  {
    if(node == 0)
      return end();

    for(size_t idx = 0; idx != d_storages.size(); ++idx)
    {
      auto iter = d_storages[idx]->find(node);
      if(iter != d_storages[idx]->end())
        return iterator(CopyPtr<Iterable>(new Iterable(iter, idx, this)));
    }

    return end();
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

