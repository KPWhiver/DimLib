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

#include "dim/scene/drawnode.hpp"
#include "dim/scene/iteratorbase.hpp"
#include "dim/util/onepair.hpp"
#include "dim/util/copyptr.hpp"

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
        DrawNodeBase &dereference();
        DrawNodeBase const &dereference() const;
        bool equal(ClonePtr<Iterable> const &other) const;

        virtual void v_increment() = 0;
        virtual DrawNodeBase &v_dereference() = 0;
        virtual DrawNodeBase const &v_dereference() const = 0;
        virtual bool v_equal(ClonePtr<Iterable> const &other) const = 0;
      };

      typedef IteratorBase<DrawNodeBase, NodeStorageBase, ClonePtr<Iterable>>  iterator;
      //typedef IteratorBase<DrawNodeBase const, NodeStorageBase const, ClonePtr<IterType>> const_iterator;

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
      void draw(ShaderScene const &state);
      iterator find(ShaderScene const &state, float x, float z);
      iterator find(float x, float z);
      void del(iterator &object);

    private:
      virtual void v_clear() = 0;
      virtual void v_draw(ShaderScene const &state) = 0;
      virtual iterator v_find(float x, float z) = 0;
      virtual iterator v_find(ShaderScene const &state, float x, float z) = 0;
      virtual void v_del(iterator &object) = 0;

    protected:
    // private functions
      size_t ownerId() const;
  };
}
}
  
#endif
