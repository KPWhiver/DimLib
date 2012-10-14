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

namespace dim
{
namespace internal
{
  /* constructors */

  template<typename RefType>
  NodeGrid<RefType>::NodeGrid(size_t gridSize, size_t key)
      :
        NodeStorageBase(key),
        d_gridSize(gridSize)
  {
    s_map[key] = this;
  }

  template<typename RefType>
  NodeGrid<RefType>::~NodeGrid()
  {
    s_map.remove(ownerId());
  }

  /* static access */
  
  template <typename RefType>
  NodeGrid<RefType> *NodeGrid<RefType>::get(size_t key)
  {
    auto iter = s_map.find(key);
    if(iter != s_map.end())
      return iter.second;

    return 0;
  }

  template <typename RefType>
  bool NodeGrid<RefType>::isPresent(size_t key)
  {
    if(s_map.find(key) != s_map.end())
      return true;
      
    return false;
  }
  
  template <typename RefType>
  void NodeGrid<RefType>::v_copy(size_t dest) const
  {
    s_map[dest] = this;
  }
  
  template <typename RefType>
  NodeStorageBase* NodeGrid<RefType>::v_clone() const
  {
    return new NodeGrid<RefType>(*this);
  }

  /* iterators */

  template <typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::begin()
  {
    for(auto mapPart = d_map.begin(); mapPart != d_map.end(); ++mapPart)
    {
      if(mapPart->second.size() != 0)
        return typename NodeGrid<RefType>::iterator({0, mapPart}, this);
    }

    return end();
  }

  template <typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::end()
  {
    return typename NodeGrid<RefType>::iterator({std::numeric_limits<size_t>::max(), d_map.end()}, this);
  }

  template <typename RefType>
  typename NodeGrid<RefType>::const_iterator NodeGrid<RefType>::begin() const
  {
    for(auto mapPart = d_map.begin(); mapPart != d_map.end(); ++mapPart)
    {
      if(mapPart->second.size() != 0)
        return typename NodeGrid<RefType>::const_iterator({0, mapPart}, this);
    }

    return end();
  }

  template <typename RefType>
  typename NodeGrid<RefType>::const_iterator NodeGrid<RefType>::end() const
  {
    return typename NodeGrid<RefType>::const_iterator({std::numeric_limits<size_t>::max(), d_map.end()}, this);
  }

  template<typename RefType>
  void NodeGrid<RefType>::increment(typename NodeGrid<RefType>::IterType *iter) const
  {
    size_t next = iter->listIdx + 1;

    for(auto mapPart = iter->mapIterator; mapPart != d_map.end(); ++mapPart)
    {
      if(next < mapPart->second.size())
      {
        *iter = {next, mapPart};
        return;
      }

      next = 0;
    }

    *iter = end().iter();
  }

  template<typename RefType>
  RefType &NodeGrid<RefType>::dereference(typename NodeGrid<RefType>::IterType const &iter)
  {
    return iter.mapIterator->second[iter.listIdx];
  }

  template<typename RefType>
  RefType const &NodeGrid<RefType>::dereference(typename NodeGrid<RefType>::IterType const &iter) const
  {
    return iter.mapIterator->second[iter.listIdx];
  }

  template<typename RefType>
  bool equal(typename NodeGrid<RefType>::IterType const &lhs, typename NodeGrid<RefType>::IterType const &rhs) const
  {
    return lhs.listIdx == rhs.listIdx && lhs.mapIterator == rhs.mapIterator;
  }

  template <typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_begin()
  {
    IterType *ptr = new IterType(begin().iter());
    return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }
  template <typename RefType>
  NodeStorageBase::const_iterator NodeGrid<RefType>::v_begin() const
  {
    IterType *ptr = new IterType(begin().iter());
    return NodeStorageBase::const_iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }

  template <typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_begin()
  {
    IterType *ptr = new IterType(end().iter());
    return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }
  template <typename RefType>
  NodeStorageBase::const_iterator NodeGrid<RefType>::v_begin() const
  {
    IterType *ptr = new IterType(end().iter());
    return NodeStorageBase::const_iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_increment(ClonePtr<NodeStorageBase::IterType> *iter) const
  {
    // We can be certain that this goes okay, or this function would not have been called
    NodeStorageBase::IterType *ptr = *iter;
    increment(reinterpret_cast<IterType*>(ptr);
  }

  template<typename RefType>
  DrawNodeBase &NodeGrid<RefType>::v_dereference(ClonePtr<NodeStorageBase::IterType> const &iter)
  {
    NodeStorageBase::IterType *ptr = iter;
    return dereference(reinterpret_cast<IterType>(*ptr));
  }

  template<typename RefType>
  DrawNodeBase const &NodeGrid<RefType>::v_dereference(ClonePtr<NodeStorageBase::IterType> const &iter) const
  {
    NodeStorageBase::IterType *ptr = iter;
    return dereference(reinterpret_cast<IterType>(*ptr));
  }

  template<typename RefType>
  bool NodeGrid<RefType>::v_equal(ClonePtr<NodeStorageBase::IterType> const &lhs, ClonePtr<NodeStorageBase::IterType> const &rhs) const
  {
    NodeStorageBase::IterType *ptr1 = lhs;
    NodeStorageBase::IterType *ptr2 = rhs;
    return equal(reinterpret_cast<IterType>(*ptr1), reinterpret_cast<IterType>(*ptr2));
  }

  
  /* private functions */

  template <typename RefType>
  size_t NodeGrid<RefType>::count() const
  {
    size_t count = 0;
    for(auto mapPart : d_map)
      count += mapPart.second.size();

    return count;
  }
  
  /* regular functions */

  template<typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::add(bool changing, RefType *object)
  {
    int xloc, zloc;
    xloc = object->coor().x / d_gridSize;
    zloc = object->coor().z / d_gridSize;

    auto iter = d_map.find(Key(xloc, zloc));
    if(iter == d_map.end())
      d_map.insert(make_pair(Key(xloc, zloc), PtrVector<RefType>([](RefType *ptr){return ptr->clone();})));

    //std::pair<size_t, DrawNode::Key> id(list.size(), DrawNode::Key(xloc, zloc));

    //object.d_id = id;
    //RefType tmp(object);
    //tmp.d_id = id;

    auto iter = lower_bound(list.begin(), list.end(), object, [](DrawNodeBase const *lhs, DrawNodeBase const *rhs)
                            {
                              return ShaderScene{lhs->scene(), lhs->shader()} < ShaderScene{lhs->scene(), rhs->shader()};
                            });

    iter = list.insert(iter, object);

    // TODO improve efficiency
    size_t idx = 0;
    for(auto iterate = list.begin(); iterate != list.end(); ++iterate, ++idx)
    {
      if(iterate == iter)
        break;
    }

    return typename NodeGrid<RefType>::iterator(idx, d_map.find(Key(xloc, zloc)));
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_clear()
  {
    d_map.clear();
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_draw(DrawState const &state)
  {
    for(auto mapPart : d_map)
    {
      auto DrawNodeBase = lower_bound(mapPart.second.begin(), mapPart.second.end(), state, [](DrawNodeBase const *lhs, DrawState const &rhs)
                              {
                                return lhs->drawState() < rhs;
                              });

      for(; DrawNodeBase->drawState() == state; ++DrawNodeBase)
        DrawNodeBase->draw();
    }
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_del(NodeStorageBase::iterator &object)
  {
    setChanged(true);
    auto mapPart = d_map.find(object.iter().second);
    mapPart->second.remove(mapPart->second.begin() + object.iter().first);
  }

  template<typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_find(float x, float z)
  {
    auto iter = find(x, z);
    return NodeStorageBase::iterator(std::make_pair(iter->first, iter->second->first), this);
  }

  template<typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_find(DrawState const &state, float x, float z)
  {
    int xloc, zloc;
    xloc = x / d_gridSize;
    zloc = z / d_gridSize;

    auto mapPart = d_map.find(Key(xloc, zloc));
    if(mapPart == d_map.end())
      return end();

    auto DrawNodeBase = lower_bound(mapPart.second.begin(), mapPart.second.end(), state, [](DrawNodeBase const *lhs, DrawState const &rhs)
                            {
                              return lhs->drawState() < rhs;
                            });

    for(size_t idx = 0; DrawNodeBase->drawState() == state; ++DrawNodeBase, ++idx)
    {
      glm::vec3 coor = mapPart->second[idx].coor();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < mapPart->second[idx].d_radius * mapPart->second[idx].d_radius)
        return typename NodeGrid<RefType>::iterator(make_pair(idx, mapPart->first), this);
    }
    return end();
  }

  template<typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::find(float x, float z)
  {
    int xloc, zloc;
    xloc = x / d_gridSize;
    zloc = z / d_gridSize;

    auto mapPart = d_map.find(Key(xloc, zloc));
    if(mapPart == d_map.end())
      return end();

    for(size_t idx = 0; idx != mapPart->second.size(); ++idx)
    {
      glm::vec3 coor = mapPart->second[idx].coor();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < mapPart->second[idx].d_radius * mapPart->second[idx].d_radius)
        return typename NodeGrid<RefType>::iterator(make_pair(idx, mapPart), this);
    }
    return end();
  }
}
}
