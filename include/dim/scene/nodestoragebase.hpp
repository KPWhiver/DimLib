// nodestoragebase.hpp
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

#ifndef NODESTORAGEBASE_HPP
#define NODESTORAGEBASE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <tuple>

#include "dim/scene/nodebase.hpp"
#include "dim/scene/iteratorbase.hpp"
#include "dim/util/onepair.hpp"
#include "dim/util/copyptr.hpp"

namespace dim
{

  class ShaderScene
  {
    private:
      std::vector<GLuint> d_shaderIds;
      
      static std::unordered_map<GLuint, Shader> s_shaderMap;
      
      DrawState d_state;

    public:
      ShaderScene(NodeBase const &node, size_t sceneIdx, size_t numOfShaders)
      :
        d_shaderIds(),
        d_state(node.scene()[sceneIdx])
      {
        for(size_t shader = 0; shader != numOfShaders; ++shader)
        {
          GLuint shaderId = node.shader(shader).id();
        
          d_shaderIds.push_back(shaderId); // Will this work?
          if(s_shaderMap.find(shaderId) == s_shaderMap.end())
            s_shaderMap.insert(std::make_pair(shaderId, node.shader(shader)));
        }
      }
    
      size_t numOfShaders() const
      {
        return d_shaderIds.size();
      }
      
      Shader &shader(size_t idx)
      {
        auto iter = s_shaderMap.find(d_shaderIds.at(idx));
        if(iter == s_shaderMap.end())
          log(__FILE__, __LINE__, LogType::error, "This should never throw, bug in the ShaderScene code");
        
        return iter->second;
      }
      
      Shader const &shader(size_t idx) const
      {
        auto iter = s_shaderMap.find(d_shaderIds.at(idx));
        if(iter == s_shaderMap.end())
          log(__FILE__, __LINE__, LogType::error, "This should never throw, bug in the ShaderScene code");
        
        return iter->second;
      }
      
      DrawState &state()
      {
        return d_state;
      }
      
      DrawState const &state() const
      {
        return d_state;
      }

      bool operator==(ShaderScene const &other) const
      {
        if(other.numOfShaders() != numOfShaders())
          log(__FILE__, __LINE__, LogType::error, "This should never throw, bug in the ShaderScene code");
      
        for(size_t shader = 0; shader != numOfShaders(); ++shader)
        {
          if(d_shaderIds[shader] != other.d_shaderIds[shader])
            return false;
        }
      
        return d_state == other.d_state;
      }

      bool operator<(ShaderScene const &other) const
      {
        if(other.numOfShaders() != numOfShaders())
          log(__FILE__, __LINE__, LogType::error, "This should never throw, bug in the ShaderScene code");
      
        for(size_t shader = 0; shader != numOfShaders(); ++shader)
        {
          if(d_shaderIds[shader] < other.d_shaderIds[shader])
            return true;
          if(not d_shaderIds[shader] == other.d_shaderIds[shader])
            return false;
        }

        if(d_state < other.d_state)
          return true;

        return false;
      }
  };

namespace internal
{
  class NodeStorageBase
  {
      size_t d_ownerId;
      
    public:
    // constructors
      virtual ~NodeStorageBase();
      explicit NodeStorageBase(size_t ownerId);

      NodeStorageBase* clone() const;

      void copy(size_t dest);

    private:
      virtual void v_copy(size_t dest) = 0;
      virtual NodeStorageBase* v_clone() const = 0;

    public:
    // iterators
      class Iterable
      {
        public:
        virtual Iterable* clone() const = 0;
        virtual ~Iterable(){};

        void increment();
        NodeBase &dereference();
        NodeBase const &dereference() const;
        bool equal(ClonePtr<Iterable> const &other) const;

        virtual void v_increment() = 0;
        virtual NodeBase &v_dereference() = 0;
        virtual NodeBase const &v_dereference() const = 0;
        virtual bool v_equal(ClonePtr<Iterable> const &other) const = 0;
      };

      typedef IteratorBase<NodeBase, NodeStorageBase, ClonePtr<Iterable>>  iterator;
      //typedef IteratorBase<NodeBase const, NodeStorageBase const, ClonePtr<IterType>> const_iterator;

      iterator begin();
      iterator end();
      //const_iterator begin() const;
      //const_iterator end() const;

    private:
      virtual iterator v_begin() = 0;
      //virtual const_iterator v_begin() const = 0;
      virtual iterator v_end() = 0;
      //virtual const_iterator v_end() const = 0;

    public:
    // regular functions
      void clear();
      void draw(ShaderScene const &state, size_t renderMode);
      iterator find(ShaderScene const &state, float x, float z);
      iterator find(float x, float z);
      iterator find(NodeBase* node);
      void del(iterator &object);
      bool updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to);

    private:
      virtual void v_clear() = 0;
      virtual void v_draw(ShaderScene const &state, size_t renderMode) = 0;
      virtual iterator v_find(NodeBase *node) = 0;
      virtual iterator v_find(float x, float z) = 0;
      virtual iterator v_find(ShaderScene const &state, float x, float z) = 0;
      virtual void v_del(iterator &object) = 0;
      virtual bool v_updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to) = 0;

    protected:
    // private functions
      size_t ownerId() const;
  };
}
}
  
#endif
