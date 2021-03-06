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
  void NodeGrid<RefType>::setGridSize(size_t gridSize)
  {
    d_gridSize = gridSize;
  }

  template<typename RefType>
  void NodeGrid<RefType>::setNumOfShaders(size_t numOfShaders)
  {
    d_numOfShaders = numOfShaders;
  }

  template<typename RefType>
  NodeGrid<RefType>::NodeGrid()
      :
        d_gridSize(0),
        d_numOfShaders(0)
  {
  }

  /* iterators */

  template<typename RefType>
  typename NodeGrid<RefType>::Iterable* NodeGrid<RefType>::Iterable::clone() const
  {
    return new NodeGrid<RefType>::Iterable(*this);
  }

  template <typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::begin()
  {
    for(auto mapPart = d_map.begin(); mapPart != d_map.end(); ++mapPart)
    {
      if(mapPart->second.size() != 0)
        return typename NodeGrid<RefType>::iterator(CopyPtr<Iterable>(new NodeGrid<RefType>::Iterable(0, mapPart, this)));
    }

    return end();
  }

  template <typename RefType>
  typename NodeGrid<RefType>::iterator NodeGrid<RefType>::end()
  {
    return typename NodeGrid<RefType>::iterator(CopyPtr<Iterable>(new NodeGrid<RefType>::Iterable(std::numeric_limits<size_t>::max(), d_map.end(), this)));
  }

  /*template <typename RefType>
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
  }*/

  template<typename RefType>
  NodeGrid<RefType>::Iterable::Iterable(size_t idx, typename Storage::iterator iter, NodeGrid<RefType> *container)
  :
    d_listIdx(idx),
    d_mapIterator(iter),
    d_container(container)
  {
  }

  template<typename RefType>
  void NodeGrid<RefType>::Iterable::increment()
  {
    size_t next = d_listIdx + 1;

    for(auto mapPart = d_mapIterator; mapPart != d_container->d_map.end(); ++mapPart)
    {
      if(next < mapPart->second.size())
      {
        d_listIdx = next;
        d_mapIterator = mapPart;
        return;
      }

      next = 0;
    }

    d_listIdx = d_container->end().iterable()->d_listIdx;
    d_mapIterator = d_container->end().iterable()->d_mapIterator;
  }

  template<typename RefType>
  RefType &NodeGrid<RefType>::Iterable::dereference()
  {
    return *d_mapIterator->second[d_listIdx];
  }

  template<typename RefType>
  RefType const &NodeGrid<RefType>::Iterable::dereference() const
  {
    return *d_mapIterator->second[d_listIdx];
  }

  template<typename RefType>
  bool NodeGrid<RefType>::Iterable::equal(CopyPtr<NodeGrid<RefType>::Iterable> const &other) const
  {
    return d_listIdx == other->d_listIdx && d_mapIterator == other->d_mapIterator;
  }

  template<typename RefType>
  void NodeGrid<RefType>::Iterable::erase()
  {
    auto &list = d_mapIterator->second;
    list.erase(list.begin() + d_listIdx);
  }

  template <typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_begin()
  {
    Iterable *ptr = begin().iterable()->clone();
    return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::Iterable>(ptr));
  }
  /*template <typename RefType>
  NodeStorageBase::const_iterator NodeGrid<RefType>::v_begin() const
  {
    IterType *ptr = new IterType(begin().iter());
    return NodeStorageBase::const_iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }*/

  template <typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_end()
  {
    Iterable *ptr = end().iterable()->clone();
    return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::Iterable>(ptr));
  }
  /*template <typename RefType>
  NodeStorageBase::const_iterator NodeGrid<RefType>::v_end() const
  {
    IterType *ptr = new IterType(end().iter());
    return NodeStorageBase::const_iterator(ClonePtr<NodeStorageBase::IterType>(ptr), this);
  }*/

  template<typename RefType>
  void NodeGrid<RefType>::Iterable::v_increment()
  {
    increment();
  }

  template<typename RefType>
  NodeBase &NodeGrid<RefType>::Iterable::v_dereference()
  {
    return dereference();
  }

  template<typename RefType>
  NodeBase const &NodeGrid<RefType>::Iterable::v_dereference() const
  {
    return dereference();
  }

  template<typename RefType>
  bool NodeGrid<RefType>::Iterable::v_equal(ClonePtr<NodeStorageBase::Iterable> const &other) const
  {
    NodeStorageBase::Iterable *ptr = other->clone();

    return equal(CopyPtr<Iterable>(reinterpret_cast<Iterable *>(ptr)));
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
    xloc = object->location().x / d_gridSize;
    zloc = object->location().z / d_gridSize;

    auto list = d_map.find(Key(xloc, zloc));
    if(list == d_map.end())
      list = d_map.insert(make_pair(Key(xloc, zloc), PtrVector<RefType>{})).first;

    //std::pair<size_t, DrawNode::Key> id(list.size(), DrawNode::Key(xloc, zloc));

    //object.d_id = id;
    //RefType tmp(object);
    //tmp.d_id = id;

    auto iter = lower_bound(list->second.begin(), list->second.end(), object, [&](NodeBase const *lhs, NodeBase const *rhs) -> bool
                            {
                              for(size_t shader = 0; shader != d_numOfShaders; ++shader)
                              {
                                if(lhs->shader(shader).id() < rhs->shader(shader).id())
                                  return true;
                                if(lhs->shader(shader).id() != rhs->shader(shader).id())
                                  return false;
                              }

                              return lhs->scene() < rhs->scene();
                            });

    iter = list->second.insert(iter, object);

    // TODO improve efficiency
    size_t idx = distance(list->second.begin(), iter);

    return typename NodeGrid<RefType>::iterator(CopyPtr<Iterable>(new Iterable(idx, d_map.find(Key(xloc, zloc)), this)));
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_clear()
  {
    d_map.clear();
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_draw(ShaderScene const &scene, size_t renderMode)
  {
    //TODO optimize optimize optimize
    for(auto mapPart : d_map)
    {
      for(NodeBase *node : mapPart.second)
      {
        for(size_t idx = 0; idx != node->scene().size(); ++idx)
        {
          if(node->scene()[idx] == scene.state())
          {
            glm::mat3 normalMatrix(glm::inverseTranspose(node->matrix()));
            node->shader(renderMode).set("in_mat_model", node->matrix());
            node->shader(renderMode).set("in_mat_normal", normalMatrix);

            scene.state().mesh().draw();
            break;
          }
        }
      }
    }
  }

  template<typename RefType>
  void NodeGrid<RefType>::v_del(NodeStorageBase::iterator &object)
  {
    NodeStorageBase::Iterable *ptr = object.iterable().get();
    Iterable *iterPair = reinterpret_cast<Iterable*>(ptr);

    iterPair->erase();
  }

  template<typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_find(float x, float z)
  {
    auto iter = find(x, z);
    Iterable *ptr = iter.iterable()->clone();
    return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::Iterable>(ptr));
  }

  template<typename RefType>
  NodeStorageBase::iterator NodeGrid<RefType>::v_find(ShaderScene const &state, float x, float z)
  {
    /*int xloc, zloc;
    xloc = x / d_gridSize;
    zloc = z / d_gridSize;

    auto mapPart = d_map.find(Key(xloc, zloc));
    if(mapPart == d_map.end())
      return end();

    auto NodeBase = lower_bound(mapPart.second.begin(), mapPart.second.end(), state, [](NodeBase const *lhs, DrawState const &rhs)
                            {
                              return lhs->drawState() < rhs;
                            });

    for(size_t idx = 0; NodeBase->drawState() == state; ++NodeBase, ++idx)
    {
      glm::vec3 coor = mapPart->second[idx].location();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < mapPart->second[idx].d_radius * mapPart->second[idx].d_radius)
        return typename NodeGrid<RefType>::iterator(make_pair(idx, mapPart->first), this);
    }*/
    return v_end();
  }

  template<typename RefType>
  typename NodeStorageBase::iterator NodeGrid<RefType>::v_find(NodeBase* node)
  {
    int xloc, zloc;
    xloc = node->location().x / d_gridSize;
    zloc = node->location().z / d_gridSize;

    auto mapPart = d_map.find(Key(xloc, zloc));
    if(mapPart == d_map.end())
      return NodeStorageBase::end();

    for(size_t idx = 0; idx != mapPart->second.size(); ++idx)
    {
      if(mapPart->second[idx] == node)
        return NodeStorageBase::iterator(ClonePtr<NodeStorageBase::Iterable>(new NodeGrid::Iterable(idx, mapPart, this)));
    }

    return NodeStorageBase::end();
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
      glm::vec3 coor = mapPart->second[idx]->location();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < 1)
        return typename NodeGrid<RefType>::iterator(CopyPtr<Iterable>(new NodeGrid::Iterable(idx, mapPart, this)));
    }

    return end();
  }

  template<typename RefType>
  bool NodeGrid<RefType>::v_updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to)
  {
    // check if we have to delete from this
    int xloc, zloc;
    xloc = from.x / d_gridSize;
    zloc = from.z / d_gridSize;

    auto mapPart = d_map.find(Key(xloc, zloc));
    if(mapPart == d_map.end())
      return false; // it is not in this nodegrid

    for(size_t idx = 0; idx != mapPart->second.size(); ++idx)
    {
      if(mapPart->second[idx] == node)
      {
        mapPart->second.erase(mapPart->second.begin() + idx);

        add(true, reinterpret_cast<RefType *>(node));

        return true; // it is here
      }
    }

    return false;
  }
}
}
