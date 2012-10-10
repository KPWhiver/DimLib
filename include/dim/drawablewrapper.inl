// drawablewrapperimplement.hpp
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
  DrawableWrapper<RefType>::DrawableWrapper(size_t gridSize, size_t key)
      :
        DrawableWrapper<Drawable>(gridSize, key)
  {
    s_map[key] = this;
  }

  template<typename RefType>
  DrawableWrapper<RefType>::~DrawableWrapper()
  {
    s_map.remove(ownerId());
  }

  /* static access */
  
  template <typename RefType>
  DrawableWrapper<RefType> *DrawableWrapper<RefType>::get(size_t key)
  {
    auto iter = s_map.find(key);
    if(iter != s_map.end())
      return iter.second;

    return 0;
  }

  template <typename RefType>
  bool DrawableWrapper<RefType>::isPresent(size_t key)
  {
    if(s_map.find(key) != s_map.end())
      return true;
      
    return false;
  }
  
  template <typename RefType>
  void DrawableWrapper<RefType>::v_copy(size_t dest) const
  {
    s_map[dest] = this;
  }
  
  template <typename RefType>
  DrawableWrapper<Drawable>* DrawableWrapper<RefType>::v_clone() const
  {
    return new DrawableWrapper<RefType>(*this);
  }

  /* iterators */

  template <typename RefType>
  typename DrawableWrapper<RefType>::iterator DrawableWrapper<RefType>::begin()
  {
    for(auto mapPart = d_map.begin(); mapPart != d_map.end(); ++mapPart)
    {
      if(mapPart->second.size() != 0)
        return typename DrawableWrapper<RefType>::iterator(std::make_pair(0, mapPart), this);
    }

    return end();
  }

  template <typename RefType>
  typename DrawableWrapper<RefType>::iterator DrawableWrapper<RefType>::end()
  {
    return typename DrawableWrapper<RefType>::iterator(std::make_pair(std::numeric_limits<size_t>::max(), d_map.end()), this);
  }

  template <typename RefType>
  typename DrawableWrapper<RefType>::const_iterator DrawableWrapper<RefType>::begin() const
  {
    for(auto mapPart = d_map.begin(); mapPart != d_map.end(); ++mapPart)
    {
      if(mapPart->second.size() != 0)
        return typename DrawableWrapper<RefType>::const_iterator(std::make_pair(0, mapPart), this);
    }

    return end();
  }

  template <typename RefType>
  typename DrawableWrapper<RefType>::const_iterator DrawableWrapper<RefType>::end() const
  {
    return typename DrawableWrapper<RefType>::const_iterator(std::make_pair(std::numeric_limits<size_t>::max(), d_map.end()), this);
  }

  template<typename RefType>
  typename DrawableWrapper<RefType>::IterType DrawableWrapper<RefType>::increment(typename DrawableWrapper<RefType>::IterType const &iter) const
  {
    size_t next = iter.first + 1;

    for(auto mapPart = iter.second; mapPart != d_map.end(); ++mapPart)
    {
      if(next < mapPart->second.size())
        return std::make_pair(next, mapPart);

      next = 0;
    }

    return end().iter();
  }

  template<typename RefType>
  RefType &DrawableWrapper<RefType>::dereference(typename DrawableWrapper<RefType>::IterType const &iter)
  {
    return iter.second->second[iter.first];
  }

  template<typename RefType>
  RefType const &DrawableWrapper<RefType>::dereference(typename DrawableWrapper<RefType>::IterType const &iter) const
  {
    return iter.second->second[iter.first];
  }

  template <typename RefType>
  DrawableWrapper<Drawable>::iterator DrawableWrapper<RefType>::v_begin()
  {
    IterType &ref = begin().iter();
    return DrawableWrapper<Drawable>::iterator(std::make_pair(ref.first, ref.second->first), this);
  }
  template <typename RefType>
  DrawableWrapper<Drawable>::const_iterator DrawableWrapper<RefType>::v_begin() const
  {
    IterType const &ref = begin().iter();
    return DrawableWrapper<Drawable>::const_iterator(std::make_pair(ref.first, ref.second->first), this);
  }

  template<typename RefType>
  DrawableWrapper<Drawable>::IterType DrawableWrapper<RefType>::v_increment(DrawableWrapper<Drawable>::IterType const &iter) const
  {
    IterType const val = increment(std::make_pair(iter.first, d_map.find(iter.second)));
    return std::make_pair(val.first, val.second->first);
  }

  template<typename RefType>
  Drawable &DrawableWrapper<RefType>::v_dereference(DrawableWrapper<Drawable>::IterType const &iter)
  {
    return dereference(std::make_pair(iter.first, d_map.find(iter.second)));
  }

  template<typename RefType>
  Drawable const &DrawableWrapper<RefType>::v_dereference(DrawableWrapper<Drawable>::IterType const &iter) const
  {
    return dereference(std::make_pair(iter.first, d_map.find(iter.second)));
  }

  
  /* private functions */

  template <typename RefType>
  size_t DrawableWrapper<RefType>::count() const
  {
    size_t count = 0;
    for(auto mapPart : d_map)
      count += mapPart.second.size();

    return count;
  }
  
  /* regular functions */

  template<typename RefType>
  typename DrawableWrapper<RefType>::iterator DrawableWrapper<RefType>::add(bool changing, RefType *object)
  {
    int xloc, zloc;
    xloc = object->coor().x / gridSize();
    zloc = object->coor().z / gridSize();
  
    if(changing)
      setChanged(true);

    auto iter = d_map.find(Drawable::Key(xloc, zloc));
    if(iter == d_map.end())
      d_map.insert(make_pair(Drawable::Key(xloc, zloc), PtrVector<RefType>([](RefType *ptr){return ptr->clone();})));

    //std::pair<size_t, Drawable::Key> id(list.size(), Drawable::Key(xloc, zloc));

    //object.d_id = id;
    //RefType tmp(object);
    //tmp.d_id = id;

    auto iter = lower_bound(list.begin(), list.end(), object, [](Drawable const *lhs, Drawable const *rhs)
                            {
                              return Drawable{lhs->scene(), lhs->shader()} < rhs->shader();
                            });

    iter = list.insert(iter, object);

    // TODO improve efficiency
    size_t idx = 0;
    for(auto iterate = list.begin(); iterate != list.end(); ++iterate, ++idx)
    {
      if(iterate == iter)
        break;
    }

    return typename DrawableWrapper<RefType>::iterator(idx, d_map.find(Drawable::Key(xloc, zloc)));
  }

  template<typename RefType>
  void DrawableWrapper<RefType>::v_save(std::string const &filename)
  {
    if(changed() == false)
      return;

    std::ofstream file(filename.c_str(), std::fstream::trunc);
    if(file.is_open() == false)
    {
      file.close();
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + filename);
    }

    file << count() << '\n';
    file.precision(0);
    file.setf(std::fstream::fixed);

    for(RefType &drawable : *this)
      file << drawable;

    file.close();
  }

  template<typename RefType>
  void DrawableWrapper<RefType>::v_clear()
  {
    d_map.clear();
  }

  template<typename RefType>
  void DrawableWrapper<RefType>::v_draw(DrawState const &state)
  {
    for(auto mapPart : d_map)
    {
      auto drawable = lower_bound(mapPart.second.begin(), mapPart.second.end(), state, [](Drawable const *lhs, DrawState const &rhs)
                              {
                                return lhs->drawState() < rhs;
                              });

      for(; drawable->drawState() == state; ++drawable)
        drawable->draw();
    }
  }

  template<typename RefType>
  void DrawableWrapper<RefType>::v_del(DrawableWrapper<Drawable>::iterator &object)
  {
    setChanged(true);
    auto mapPart = d_map.find(object.iter().second);
    mapPart->second.remove(mapPart->second.begin() + object.iter().first);
  }

  template<typename RefType>
  DrawableWrapper<Drawable>::iterator DrawableWrapper<RefType>::v_find(float x, float z)
  {
    auto iter = find(x, z);
    return DrawableWrapper<Drawable>::iterator(std::make_pair(iter->first, iter->second->first), this);
  }

  template<typename RefType>
  DrawableWrapper<Drawable>::iterator DrawableWrapper<RefType>::v_find(DrawState const &state, float x, float z)
  {
    int xloc, zloc;
    xloc = x / gridSize();
    zloc = z / gridSize();

    auto mapPart = d_map.find(Drawable::Key(xloc, zloc));
    if(mapPart == d_map.end())
      return end();

    auto drawable = lower_bound(mapPart.second.begin(), mapPart.second.end(), state, [](Drawable const *lhs, DrawState const &rhs)
                            {
                              return lhs->drawState() < rhs;
                            });

    for(size_t idx = 0; drawable->drawState() == state; ++drawable, ++idx)
    {
      glm::vec3 coor = mapPart->second[idx].coor();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < mapPart->second[idx].d_radius * mapPart->second[idx].d_radius)
        return typename DrawableWrapper<RefType>::iterator(make_pair(idx, mapPart->first), this);
    }
    return end();
  }

  template<typename RefType>
  typename DrawableWrapper<RefType>::iterator DrawableWrapper<RefType>::find(float x, float z)
  {
    int xloc, zloc;
    xloc = x / gridSize();
    zloc = z / gridSize();

    auto mapPart = d_map.find(Drawable::Key(xloc, zloc));
    if(mapPart == d_map.end())
      return end();

    for(size_t idx = 0; idx != mapPart->second.size(); ++idx)
    {
      glm::vec3 coor = mapPart->second[idx].coor();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < mapPart->second[idx].d_radius * mapPart->second[idx].d_radius)
        return typename DrawableWrapper<RefType>::iterator(make_pair(idx, mapPart), this);
    }
    return end();
  }
}
}
