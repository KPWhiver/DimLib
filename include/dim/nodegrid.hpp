// drawablewrapper.hpp
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

#ifndef DRAWABLEWRAPPER_HPP
#define DRAWABLEWRAPPER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "dim/nodestoragebase.hpp"

namespace dim
{
namespace internal
{
  template<typename RefType>
  class NodeGrid : public NodeStorageBase
  {
      typedef std::unordered_map<Drawable::Key, PtrVector<RefType>, std::hash<long>, std::equal_to<long>> Storage;
      
      Storage d_map;

      static std::unordered_map<size_t, NodeGrid<RefType>*> s_map;

      size_t d_gridSize;

    public:
    // constuctors
      NodeGrid(size_t gridSize, size_t key);
      ~NodeGrid();
      
    // static access
      static NodeGrid<RefType> *get(size_t key);
      static bool isPresent(size_t key);

    private:
      virtual void v_copy(size_t dest) const;
      virtual NodeStorageBase* v_clone() const;

    public:
    // iterators
      struct IterType
      {
        size_t listIdx;
        typename Storage::iterator mapIterator;

        virtual IterType* clone() const
        {
          return new IterType(*this);
        }
      };

      typedef IteratorBase<RefType, NodeGrid<RefType>, IterType> iterator;
      typedef IteratorBase<RefType const, NodeGrid<RefType> const, IterType> const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;
      
      RefType &dereference(IterType const &iter);
      RefType const &dereference(IterType const &iter) const;
      void increment(IterType *iter) const;
      bool equal(IterType const &lhs, IterType const &rhs) const;

    private:
      virtual void v_increment(ClonePtr<NodeStorageBase::IterType> *iter) const;
      virtual Drawable &v_dereference(ClonePtr<NodeStorageBase::IterType> const &iter);
      virtual Drawable const &v_dereference(ClonePtr<NodeStorageBase::IterType> const &iter) const;
      virtual bool v_equal(ClonePtr<NodeStorageBase::IterType> const &lhs, ClonePtr<NodeStorageBase::IterType> const &rhs) const;

      virtual NodeStorageBase::iterator v_begin();
      virtual NodeStorageBase::const_iterator v_begin() const;
      virtual NodeStorageBase::iterator v_end();
      virtual NodeStorageBase::const_iterator v_end() const;

    public:
    // regular functions
      iterator add(bool changing, RefType *object);
      iterator find(float x, float z);

    private:
      virtual void v_save(std::string const &filename);
      virtual void v_clear();
      virtual void v_draw(DrawState const &state);
      virtual NodeStorageBase::iterator v_find(float x, float z);
      virtual NodeStorageBase::iterator v_find(DrawState const &state, float x, float z);
      virtual void v_del(NodeStorageBase::iterator &object);

    // private functions
      size_t count() const;
  };
  
}
}
#include "nodegrid.inl"
  
#endif
