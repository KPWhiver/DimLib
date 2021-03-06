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

#include "dim/scene/nodegrid.hpp"
#include "dim/scene/scene.hpp"
#include "dim/util/ptrvector.hpp"
#include "dim/core/camera.hpp"
#include "dim/core/light.hpp"
#include "dim/util/tupleforeach.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <fstream>

#include "dim/scene/scenegraph.hpp"
#include "dim/core/shader.hpp"
#include <BulletWorldImporter/btBulletWorldImporter.h>

#include <algorithm>
#include <stdexcept>

namespace dim
{
  //class GroupNodeBase : public NodeBase
  //{
  //};

  template<typename... Types>
  class SceneGraph : public NodeBase
  {
      std::multimap<ShaderScene, internal::NodeStorageBase*> d_drawStates;

      std::vector<internal::NodeStorageBase*> d_storagePtrs;
      std::tuple<internal::NodeGrid<Types>...> d_storages;

      size_t d_gridSize;

      size_t d_numOfRenderModes;

      std::vector<Light> d_lights;

    // bullet
      btDbvtBroadphase d_broadphase;
      btDefaultCollisionConfiguration d_collisionConfiguration;
      btCollisionDispatcher d_dispatcher;
      btSequentialImpulseConstraintSolver d_solver;

      btDiscreteDynamicsWorld d_dynamicsWorld;

    public:
      virtual Shader const &shader(size_t idx) const
      {
        return Shader::defaultShader();
      }

      virtual Scene const &scene() const
      {
        static Scene defaultScene;
        return defaultScene;
      }

      virtual btRigidBody *rigidBody()
      {
        return 0;
      }

      virtual NodeBase *clone() const
      {
        return new SceneGraph(*this);
      }

    // setChanged
      void setOrientation(glm::quat const &orient);
      void setScaling(glm::vec3 const &scale);
      void setLocation(glm::vec3 const &coor);

    // iterators
      class Iterable
      {
        internal::NodeStorageBase::iterator d_iterator;
        size_t d_listIdx;
        SceneGraph *d_container;

        public:
        Iterable(internal::NodeStorageBase::iterator iter, size_t idx, SceneGraph *container);

        void erase();

        void increment();
        NodeBase &dereference();
        NodeBase const &dereference() const;
        bool equal(CopyPtr<Iterable> const &other) const;
      };
      friend Iterable;

      typedef internal::IteratorBase<NodeBase, SceneGraph, CopyPtr<Iterable>> iterator;
      //typedef internal::IteratorBase<NodeBase const, Iterable> const_iterator;

      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator begin();//
      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator end();//
      //template<typename RefType>
      //typename internal::NodeGrid<RefType>::const_iterator begin() const;//
      //template<typename RefType>
      //typename internal::NodeGrid<RefType>::const_iterator end() const;//

      iterator begin();
      iterator end();
      //const_iterator begin() const;
      //const_iterator end() const;

    // constructors

      SceneGraph(size_t numOfRenderModes, size_t gridSize = 64);

      SceneGraph(SceneGraph const &other);
      SceneGraph(SceneGraph &&tmp);

      SceneGraph &operator=(SceneGraph const &other);
      SceneGraph &operator=(SceneGraph &&tmp);

    // regular functions
      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator add(bool saved, RefType *object);

      //btDiscreteDynamicsWorld *physicsWorld();
      void addRigidBody(btRigidBody *rigidBody);
      void updateRigidBody(btRigidBody *rigidBody);
      void addAction(btActionInterface *action);
      void addGhost(btGhostObject *ghost);
      void addBulletFile(std::string const &filename);

      void addLight(Light const &light);

      template<typename RefType>
      void load(std::string const &filename);

      template<typename RefType>
      void save(std::string const &filename);
      void clear();

      void updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to) override;

      void del(SceneGraph::iterator object);
      SceneGraph::iterator get(float x, float z);

      SceneGraph::iterator get(ShaderScene const &state, float x, float z);

      SceneGraph::iterator get(NodeBase *node);

      template<typename RefType>
      typename internal::NodeGrid<RefType>::iterator get(float x, float z);

      void physicsStep(float time);

      void draw(Camera camera, size_t renderMode);

    private:
      void add(ShaderScene const &state, internal::NodeStorageBase* ptr);
      SceneGraph::iterator find(float x, float z);
  };

  // TODO fix
  template<typename... Types>
  template<typename RefType>
  void SceneGraph<Types...>::save(std::string const &filename)
  {
    std::ofstream file(filename.c_str());

    if(file.is_open() == false)
    {
      throw log(__FILE__, __LINE__, LogType::error, "Failed to load " + filename);
      return;
    }

    for(auto node = begin<RefType>(); node != end<RefType>(); ++node)
      file << *node;

    file.close();
  }

  template<typename... Types>
  template<typename RefType>
  void SceneGraph<Types...>::load(std::string const &filename)
  {
    // open the file
    std::ifstream file(filename.c_str());
    if(not file.is_open())
    {
      throw log(__FILE__, __LINE__, LogType::error, "Failed to open " + filename);
      return;
    }

    RefType *ref = new RefType;
    while(file >> *ref)
    {
      //TODO room for optimization
      add(false, ref);
      ref = new RefType;
    }
    delete ref;
    file.close();

  }

  template<typename... Types>
  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph<Types...>::add(bool saved, RefType *object)
  {
    internal::NodeGrid<RefType> &storage = dim::get<internal::NodeGrid<RefType>>(d_storages);
    object->setParent(this);

    typename internal::NodeGrid<RefType>::iterator iter = storage.add(!saved, object);

    // Add the drawstate
    for(size_t idx = 0; idx != object->scene().size(); ++idx)
      add(ShaderScene(*object, idx, d_numOfRenderModes), &storage);

    if(object->rigidBody() != 0)
      d_dynamicsWorld.addRigidBody(object->rigidBody());

    return iter;
  }

  template<typename... Types>
  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph<Types...>::get(float x, float z)
  {
    return dim::get<internal::NodeGrid<RefType>>(d_storages).find(x, z);
  }

  template<typename... Types>
  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph<Types...>::begin()
  {
    return dim::get<internal::NodeGrid<RefType>>(d_storages).begin();
  }

  template<typename... Types>
  template<typename RefType>
  typename internal::NodeGrid<RefType>::iterator SceneGraph<Types...>::end()
  {
    return dim::get<internal::NodeGrid<RefType>>(d_storages).end();
  }

  /*template<typename RefType>
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
  }*/

}

#include "scenegraph.inl"


#endif
