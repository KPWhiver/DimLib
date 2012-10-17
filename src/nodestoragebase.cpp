// nodestoragebase.cpp
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

#include "dim/nodestoragebase.hpp"

using namespace std;

namespace dim
{
namespace internal
{
  /* constructors */
  NodeStorageBase::NodeStorageBase(size_t ownerId)
      :
          d_ownerId(ownerId)
  {
  }  
  
  NodeStorageBase::~NodeStorageBase()
  {
  }
  
  void NodeStorageBase::copy(size_t dest)
  {
    v_copy(dest);
    d_ownerId = dest;
  }
  
  NodeStorageBase* NodeStorageBase::clone() const
  {
    return v_clone();
  }
  
  /* general functions */

  void NodeStorageBase::clear()
  {
    v_clear();
  }

  void NodeStorageBase::draw(ShaderScene const &state)
  {
    v_draw(state);
  }  
  
  NodeStorageBase::iterator NodeStorageBase::find(float x, float z)
  {
    return v_find(x, z);
  }

  NodeStorageBase::iterator NodeStorageBase::find(ShaderScene const &state, float x, float z)
  {
    return v_find(state, x, z);
  }

  void NodeStorageBase::del(iterator &object)
  {
    v_del(object);
  }

  /* iterators */
  void NodeStorageBase::Iterable::increment()
  {
    return v_increment();
  }

  DrawNodeBase &NodeStorageBase::Iterable::dereference()
  {
    return v_dereference();
  }

  DrawNodeBase const &NodeStorageBase::Iterable::dereference() const
  {
    return v_dereference();
  }

  bool NodeStorageBase::Iterable::equal(ClonePtr<Iterable> const &other) const
  {
    return v_equal(other);
  }

  NodeStorageBase::iterator NodeStorageBase::begin()
  {
    return v_begin();
  }

  NodeStorageBase::iterator NodeStorageBase::end()
  {
    return v_end();
  }

  //NodeStorageBase::const_iterator NodeStorageBase::begin() const
  //{
  //  return v_begin();
  //}

  //NodeStorageBase::const_iterator NodeStorageBase::end() const
  //{
  //  return v_end();
  //}

  /* private functions */
  size_t NodeStorageBase::ownerId() const
  {
    return d_ownerId;
  }
}
}
