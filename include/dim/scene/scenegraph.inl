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

namespace dim
{
  /* setChanged */
  template<typename... Types>
  void SceneGraph<Types...>::setOrientation(glm::quat const &orient)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();

    NodeBase::setOrientation(orient);
  }
  template<typename... Types>
  void SceneGraph<Types...>::setScaling(glm::vec3 const &scale)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();

    NodeBase::setScaling(scale);

  }
  template<typename... Types>
  void SceneGraph<Types...>::setLocation(glm::vec3 const &coor)
  {
    for(NodeBase &drawNode: *this)
      drawNode.setChanged();

    NodeBase::setLocation(coor);

  }

  namespace
  {
  struct Caller
  {
    std::vector<internal::NodeStorageBase*> &d_list;

    Caller(std::vector<internal::NodeStorageBase*> &list)
      :
        d_list(list)
    {}

    template<typename Type>
    void operator()(Type &storage)
    {
      d_list.push_back(&storage);
    }
  };
  }

  template<typename... Types>
  SceneGraph<Types...>::SceneGraph(size_t numOfRenderModes, size_t gridSize)
      :
          d_gridSize(gridSize),
          d_numOfRenderModes(numOfRenderModes),
          d_dispatcher(&d_collisionConfiguration),
          d_dynamicsWorld(&d_dispatcher, &d_broadphase, &d_solver, &d_collisionConfiguration)
  {
    forEach(d_storages, Caller{d_storagePtrs});

    // bullet
    d_dynamicsWorld.setGravity(btVector3(0, -10, 0));
    d_dynamicsWorld.getDispatchInfo().m_allowedCcdPenetration=0.0001f;
    d_dynamicsWorld.getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

  }

  template<typename... Types>
  SceneGraph<Types...>::SceneGraph(SceneGraph const &other)
  :
      d_drawStates(other.d_drawStates),
      d_storages(other.d_storages),
      d_gridSize(other.d_gridSize),
      d_numOfRenderModes(other.d_numOfRenderModes),
      d_lights(other.d_lights),
      d_collisionConfiguration(other.d_collisionConfiguration),
      d_dispatcher(other.d_dispatcher),
      d_solver(other.d_solver),
      d_dynamicsWorld(other.d_dynamicsWorld)
  {
    forEach(d_storages, Caller{d_storagePtrs});

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);
  }

  template<typename... Types>
  SceneGraph<Types...>::SceneGraph(SceneGraph &&tmp)
  :
      d_drawStates(move(tmp.d_drawStates)),
      d_storages(move(tmp.d_storages)),
      d_gridSize(move(tmp.d_gridSize)),
      d_numOfRenderModes(move(tmp.d_numOfRenderModes)),
      d_lights(move(tmp.d_lights)),
      d_collisionConfiguration(move(tmp.d_collisionConfiguration)),
      d_dispatcher(move(tmp.d_dispatcher)),
      d_solver(move(tmp.d_solver)),
      d_dynamicsWorld(move(tmp.d_dynamicsWorld))
  {
    forEach(d_storages, Caller{d_storagePtrs});

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);
  }

  template<typename... Types>
  SceneGraph<Types...> &SceneGraph<Types...>::operator=(SceneGraph const &other)
  {
    d_drawStates = other.d_drawStates;
    d_storages = other.d_storages;
    d_gridSize = other.d_gridSize;
    d_numOfRenderModes = other.d_numOfRenderModes;
    d_lights = other.d_lights;
    d_collisionConfiguration = other.d_collisionConfiguration;
    d_dispatcher = other.d_dispatcher;
    d_solver = other.d_solver;
    d_dynamicsWorld = other.d_dynamicsWorld;

    forEach(d_storages, Caller{d_storagePtrs});

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);

    return *this;
  }

  template<typename... Types>
  SceneGraph<Types...> &SceneGraph<Types...>::operator=(SceneGraph &&tmp)
  {
    d_drawStates = move(tmp.d_drawStates);
    d_storages = move(tmp.d_storages);
    d_gridSize = move(tmp.d_gridSize);
    d_numOfRenderModes = move(tmp.d_numOfRenderModes);
    d_lights = move(tmp.d_lights);
    d_collisionConfiguration = move(tmp.d_collisionConfiguration);
    d_dispatcher = move(tmp.d_dispatcher);
    d_solver = move(tmp.d_solver);
    d_dynamicsWorld = move(tmp.d_dynamicsWorld);

    forEach(d_storages, Caller{d_storagePtrs});

    for(NodeBase &drawNode: *this)
      drawNode.setParent(this);

    return *this;
  }

  /* iterators */
  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::begin()
  {
    for(size_t idx = 0; idx != d_storagePtrs.size(); ++idx)
    {
      internal::NodeStorageBase::iterator iter = d_storagePtrs[idx]->begin();

      if(iter != d_storagePtrs[idx]->end())
        return iterator{CopyPtr<Iterable>(new Iterable(iter, idx, this))};
    }

    return end();
  }

  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::end()
  {
    return iterator{CopyPtr<Iterable>(new Iterable(d_storagePtrs.back()->end(), d_storagePtrs.size(), this))};
  }

 /* SceneGraph::const_iterator SceneGraph::begin() const
  {
    for(size_t idx = 0; idx != d_storagePtrs.size(); ++idx)
    {
      NodeStorageBase::iterator iter = d_storagePtrs[idx]->begin();

      if(iter != d_storagePtrs[idx]->end())
        return const_iterator(make_pair(iter, idx), this);
    }

    return end();
  }

  SceneGraph::const_iterator SceneGraph::end() const
  {
    return const_iterator(IterType(d_storagePtrs.back()->end(), d_storagePtrs.size()), this);
  }*/

  template<typename... Types>
  SceneGraph<Types...>::Iterable::Iterable(internal::NodeStorageBase::iterator iter, size_t idx, SceneGraph *container)
  :
      d_iterator(iter),
      d_listIdx(idx),
      d_container(container)
  {
  }

  template<typename... Types>
  void SceneGraph<Types...>::Iterable::increment()
  {
    ++d_iterator;

    if(d_iterator == d_container->d_storagePtrs[d_listIdx]->end())
    {
      for(size_t idx = d_listIdx; idx != d_container->d_storagePtrs.size(); ++idx)
      {
        d_iterator = d_container->d_storagePtrs[idx]->begin();

        if(d_iterator != d_container->d_storagePtrs[idx]->end())
        {
          d_listIdx = idx;
          return;
        }
      }

      d_listIdx = d_container->d_storagePtrs.size();
    }
  }

  template<typename... Types>
  NodeBase &SceneGraph<Types...>::Iterable::dereference()
  {
    return *d_iterator;
  }

  template<typename... Types>
  NodeBase const &SceneGraph<Types...>::Iterable::dereference() const
  {
    return *d_iterator;
  }

  template<typename... Types>
  bool SceneGraph<Types...>::Iterable::equal(CopyPtr<Iterable> const &other) const
  {
    return d_iterator == other.get()->d_iterator && d_listIdx == other.get()->d_listIdx;
  }

  template<typename... Types>
  void SceneGraph<Types...>::Iterable::erase()
  {
    d_container->d_storagePtrs[d_listIdx]->del(d_iterator);
  }

  /* regular functions */

  //btDiscreteDynamicsWorld *SceneGraph::physicsWorld()
  //{
  //  return &d_dynamicsWorld;
  //}

  template<typename... Types>
  void SceneGraph<Types...>::addRigidBody(btRigidBody *rigidBody)
  {
    if(rigidBody != 0)
      d_dynamicsWorld.addRigidBody(rigidBody);
  }

  template<typename... Types>
  void SceneGraph<Types...>::updateRigidBody(btRigidBody *rigidBody)
  {
    if(rigidBody != 0)
      d_dynamicsWorld.updateSingleAabb(rigidBody);
  }

  template<typename... Types>
  void SceneGraph<Types...>::addAction(btActionInterface *action)
  {
    if(action != 0)
      d_dynamicsWorld.addAction(action);
  }

  template<typename... Types>
  void SceneGraph<Types...>::addGhost(btGhostObject *ghost)
  {
    if(ghost != 0)
      d_dynamicsWorld.addCollisionObject(ghost, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
  }

  template<typename... Types>
  void SceneGraph<Types...>::addBulletFile(std::string const &filename)
  {
    static btBulletWorldImporter fileLoader(&d_dynamicsWorld);
    fileLoader.loadFile(filename.c_str());
  }

  template<typename... Types>
  void SceneGraph<Types...>::addLight(Light const &light)
  {
    d_lights.push_back(light);
  }

  template<typename... Types>
  void SceneGraph<Types...>::add(ShaderScene const &state, internal::NodeStorageBase* ptr)
  {
    auto range = d_drawStates.equal_range(state);

    for(auto iter = range.first; iter != range.second; ++iter)
    {
      if(iter->second == ptr)
        return;
    }

    d_drawStates.insert(std::make_pair(state, ptr));
  }

  template<typename... Types>
  void SceneGraph<Types...>::physicsStep(float time)
  {
    uint substeps = std::max(1.0f, (1 / 60) / time) + 2;
    d_dynamicsWorld.stepSimulation(time, substeps);
  }

  template<typename... Types>
  void SceneGraph<Types...>::draw(Camera camera, size_t renderMode)
  {
    GLuint previousShader = 0;
    for(auto const &element: d_drawStates)
    {
      // TODO optimize optimize optimize
      ShaderScene const &state = element.first;

      GLuint shaderId = state.shader(renderMode).id();

      if(shaderId != previousShader)
      {
        state.shader(renderMode).use();
        previousShader = shaderId;

        camera.setAtShader(state.shader(renderMode), "in_mat_view", "in_mat_projection");

        for(Light &light: d_lights)
          light.setAtShader(state.shader(renderMode));

        state.shader(renderMode).set("in_material.diffuse", state.state().diffuseIntensity());
        state.shader(renderMode).set("in_material.ambient", state.state().ambientIntensity());
        state.shader(renderMode).set("in_material.specular", state.state().specularIntensity());
        state.shader(renderMode).set("in_material.shininess", state.state().shininess());
      }

      state.state().mesh().bind();

      for(size_t tex = 0; tex != state.state().textures().size(); ++tex)
        state.shader(renderMode).set(state.state().textures()[tex].second, state.state().textures()[tex].first, tex);

      element.second->draw(state, renderMode);

      state.state().mesh().unbind();
    }
  }

  template<typename... Types>
  void SceneGraph<Types...>::del(SceneGraph::iterator object)
  {
    if(object == end())
      return;

    object.iterable()->erase();
  }

  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::find(float x, float z)
  {
    for(size_t idx = 0; idx != d_storagePtrs.size(); ++idx)
    {
      auto iter = d_storagePtrs[idx]->find(x, z);
      if(iter != d_storagePtrs[idx]->end())
        return iterator(CopyPtr<Iterable>(new Iterable(iter, idx, this)));
    }

    return end();
  }

  template<typename... Types>
  void SceneGraph<Types...>::clear()
  {
    for(auto &element: d_storagePtrs)
      element->clear();
  }

  template<typename... Types>
  void SceneGraph<Types...>::updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to)
  {
    if(static_cast<size_t>(from.x / d_gridSize) == static_cast<size_t>(to.x / d_gridSize))
    {
      if(static_cast<size_t>(from.z / d_gridSize) == static_cast<size_t>(to.z / d_gridSize))
        return;
    }

    for(auto &storage : d_storagePtrs)
    {
      if(storage->updateNode(node, from, to))
        return;
    }
  }

  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::get(float x, float z)
  {
    return find(x, z);
  }

  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::get(NodeBase *node)
  {
    if(node == 0)
      return end();

    for(size_t idx = 0; idx != d_storagePtrs.size(); ++idx)
    {
      auto iter = d_storagePtrs[idx]->find(node);
      if(iter != d_storagePtrs[idx]->end())
        return iterator(CopyPtr<Iterable>(new Iterable(iter, idx, this)));
    }

    return end();
  }

  template<typename... Types>
  typename SceneGraph<Types...>::iterator SceneGraph<Types...>::get(ShaderScene const &state, float x, float z)
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
}


