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

#include "dim/drawable.hpp"
#include "dim/iteratorbase.hpp"
#include "dim/onepair.hpp"
#include "dim/cloneptr.hpp"

namespace dim
{
namespace internal
{
  struct DrawState
  {
      Mesh d_mesh;
      Shader d_shader;

      Mesh const &mesh() const;
      Shader const &shader() const;

      bool operator==(DrawState const &other);
      bool operator<(DrawState const &other);
  };

  class NodeStorageBase
  {
      size_t d_ownerId;
      
    public:
    // constructors
      virtual ~NodeStorageBase();
      explicit NodeStorageBase(size_t ownerId);

      NodeStorageBase<Drawable>* clone() const;

      void copy(size_t dest);

    private:
      virtual void v_copy(size_t dest) const = 0;
      virtual NodeStorageBase<Drawable>* v_clone() const = 0;

    public:
    // iterators
      struct IterType
      {
        virtual IterType* clone() const = 0;
      };

      typedef IteratorBase<Drawable, NodeStorageBase<Drawable>, ClonePtr<IterType>>  iterator;
      typedef IteratorBase<Drawable const, NodeStorageBase<Drawable> const, ClonePtr<IterType>> const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

      void increment(ClonePtr<IterType> *iter) const;
      Drawable &dereference(ClonePtr<IterType> const &iter);
      Drawable const &dereference(ClonePtr<IterType> const &iter) const;
      bool equal(IterType const &lhs, ClonePtr<IterType> const &rhs) const;

    private:
      virtual void v_increment(ClonePtr<IterType> *iter) const = 0;
      virtual Drawable &v_dereference(ClonePtr<IterType> const &iter) = 0;
      virtual Drawable const &v_dereference(ClonePtr<IterType> const &iter) const = 0;
      virtual bool v_equal(IterType const &lhs, ClonePtr<IterType> const &rhs) const = 0;

      virtual iterator v_begin() = 0;
      virtual const_iterator v_begin() const = 0;
      virtual iterator v_end() = 0;
      virtual const_iterator v_end() const = 0;

    public:
    // regular functions
      void clear();
      void draw(DrawState const &state);
      iterator find(DrawState const &state, float x, float z);
      iterator find(float x, float z);
      void del(iterator &object);

    private:
      virtual void v_clear() = 0;
      virtual void v_draw(DrawState const &state) = 0;
      virtual iterator v_find(float x, float z) = 0;
      virtual iterator v_find(DrawState const &state, float x, float z) = 0;
      virtual void v_del(iterator &object) = 0;

    protected:
    // private functions
      size_t ownerId() const;
  };
}
}
  
#endif
