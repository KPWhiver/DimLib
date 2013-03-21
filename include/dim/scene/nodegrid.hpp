// nodegrid.hpp
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

#ifndef NODEGRID_HPP
#define NODEGRID_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "dim/scene/nodestoragebase.hpp"
#include "dim/util/ptrvector.hpp"
#include "dim/util/copyptr.hpp"

namespace dim
{
namespace internal
{
  template<typename RefType>
  class NodeGrid : public NodeStorageBase
  {
      typedef Onepair<long, 10000000> Key;
      typedef std::unordered_map<Key, PtrVector<RefType>, std::hash<long>, std::equal_to<long>> Storage;
      
      Storage d_map;

      //static std::unordered_map<size_t, NodeGrid<RefType>*> s_map;

      size_t d_gridSize;
      
      size_t d_numOfShaders;

    public:
    // constuctors
      NodeGrid(size_t gridSize, size_t key, size_t numOfShaders);
      ~NodeGrid();
      
    // static access
    private:
      static std::unordered_map<size_t, NodeGrid<RefType>*> &instanceMap();

    public:
      static NodeGrid<RefType> *get(size_t key);
      static bool isPresent(size_t key);

    private:
      virtual void v_copy(size_t dest);
      virtual NodeStorageBase* v_clone() const;

    public:
    // iterators
      class Iterable : public NodeStorageBase::Iterable
      {
        size_t d_listIdx;
        typename Storage::iterator d_mapIterator;
        NodeGrid<RefType> *d_container;

        public:

        Iterable(size_t idx, typename Storage::iterator iter, NodeGrid<RefType> *d_container);

        virtual Iterable* clone() const;

        RefType &dereference();
        RefType const &dereference() const;
        void increment();
        bool equal(CopyPtr<Iterable> const &other) const;
        void erase();

        virtual void v_increment();
        virtual NodeBase &v_dereference();
        virtual NodeBase const &v_dereference() const;
        virtual bool v_equal(ClonePtr<NodeStorageBase::Iterable> const &other) const;
      };
      friend Iterable;

      typedef IteratorBase<RefType, NodeGrid<RefType>, CopyPtr<Iterable>> iterator;
      //typedef IteratorBase<RefType const, NodeGrid<RefType> const, ConstIterType> const_iterator;

      iterator begin();
      iterator end();
      //const_iterator begin() const;
      //const_iterator end() const;
    private:
      virtual NodeStorageBase::iterator v_begin();
      //virtual NodeStorageBase::const_iterator v_begin() const;
      virtual NodeStorageBase::iterator v_end();
      //virtual NodeStorageBase::const_iterator v_end() const;

    public:
    // regular functions
      iterator add(bool changing, RefType *object);
      iterator find(float x, float z);

    private:
      void v_clear() override;
      void v_draw(ShaderScene const &state, size_t renderMode) override;
      NodeStorageBase::iterator v_find(NodeBase *node) override;
      NodeStorageBase::iterator v_find(float x, float z) override;
      NodeStorageBase::iterator v_find(ShaderScene const &state, float x, float z) override;
      void v_del(NodeStorageBase::iterator &object) override;
      bool v_updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to) override;

    // private functions
      size_t count() const;
  };
  
}
}
#include "nodegrid.inl"
  
#endif
