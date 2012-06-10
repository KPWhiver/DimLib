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

  /* static access */
  
  template <typename RefType>
  Storage &DrawableWrapper__<RefType>::get(DrawMap* key)
  {
    return s_map[key];
  }
  
  template <typename RefType>
  void DrawableWrapper__<RefType>::remove(DrawMap* key)
  {
    s_map.remove(key);
  }
  
  template <typename RefType>
  void DrawableWrapper__<RefType>::copy(DrawMap* source, DrawMap* dest)
  {
    s_map[dest] = s_map[source];
  }
 
  template <typename RefType>
  void DrawableWrapper__<RefType>::move(DrawMap* source, DrawMap* dest)
  {
    s_map[dest] = std::move(s_map[source]);
    s_map.remove(source);
  }
 
  template <typename RefType>
  bool DrawableWrapper__<RefType>::isPresent(DrawMap* key)
  {
    if(s_map.get(key) != s_map.end())
      return true;
      
    return false;
  }

  template <typename RefType>
  void DrawableWrapper__<RefType>::v_remove(DrawMap* key) const
  {
    remove(key);
  }
  
  template <typename RefType>
  void DrawableWrapper__<RefType>::v_copy(DrawMap* source, DrawMap* dest) const
  {
    copy(source, dest);
  }
 
  template <typename RefType>
  void DrawableWrapper__<RefType>::v_move(DrawMap* source, DrawMap* dest) const
  {
    move(source, dest);
  }
  
  template <typename RefType>
  DrawableWrapper__<Drawable> DrawableWrapper__<RefType>::v_clone() const
  {
    return new DrawableWrapper__<RefType>(*this);
  }

  /*template<typename RefType>
  DrawableWrapper__<RefType> &DrawableWrapper__<RefType>::instance()
  {
    static DrawableWrapper__<RefType> lastInstance(0);
    return lastInstance;
  }*/

  /* iterators */

  template <typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawableWrapper__<RefType>::begin()
  {
    for(auto mapPart : *d_map)
    {
      for(RefType &drawable : mapPart.second)
      {
        if(DrawableWrapper__<RefType>::iterator(drawable.d_id, this) != end())
          return typename DrawableWrapper__<RefType>::iterator(drawable.d_id, this);
      }
    }

    return end();
  }

  template <typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawableWrapper__<RefType>::end()
  {
    return typename DrawableWrapper__<RefType>::iterator(typename DrawableWrapper__<RefType>::IdType(std::numeric_limits<size_t>::max(), Drawable::Key()), 0);
  }

  template <typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator DrawableWrapper__<RefType>::begin() const
  {
   for(auto mapPart : *d_map)
    {
      for(RefType &drawable : mapPart.second)
      {
        if(DrawableWrapper__<RefType>::const_iterator(drawable.d_id, this) != end())
          return typename DrawableWrapper__<RefType>::const_iterator(drawable.d_id, this);
      }
    }

    return end();
  }

  template <typename RefType>
  typename DrawableWrapper__<RefType>::const_iterator DrawableWrapper__<RefType>::end() const
  {
    return typename DrawableWrapper__<RefType>::const_iterator(typename DrawableWrapper__<RefType>::IdType(std::numeric_limits<size_t>::max(), Drawable::Key()), 0);
  }

  template <typename RefType>
  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<RefType>::v_begin()
  {
    return DrawableWrapper__<Drawable>::iterator(begin().id(), this);
  }
  template <typename RefType>
  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<RefType>::v_end()
  {
    return DrawableWrapper__<Drawable>::iterator(end().id(), 0);
  }
  template <typename RefType>
  DrawableWrapper__<Drawable>::const_iterator DrawableWrapper__<RefType>::v_begin() const
  {
    return DrawableWrapper__<Drawable>::const_iterator(begin().id(), this);
  }
  template <typename RefType>
  DrawableWrapper__<Drawable>::const_iterator DrawableWrapper__<RefType>::v_end() const
  {
    return DrawableWrapper__<Drawable>::const_iterator(end().id(), 0);
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::IdType DrawableWrapper__<RefType>::v_nextId(
      typename DrawableWrapper__<RefType>::IdType const &id) const
  {
    size_t start = id.first;

    for(auto mapPart = d_map->find(id.second); mapPart != d_map->end(); ++mapPart)
    {
      for(size_t idx = start; idx != mapPart->second.size(); ++idx)
      {
        if(mapPart->second[idx].d_id != end().id())
          return std::make_pair(idx, mapPart->first);
      }
      start = 0;
    }
    return end().id();
  }

  template<typename RefType>
  RefType &DrawableWrapper__<RefType>::getFromId(typename DrawableWrapper__<RefType>::IdType const &id)
  {
    auto mapPart = d_map->find(id.second);
    return mapPart->second[id.first];
  }

  template<typename RefType>
  RefType const &DrawableWrapper__<RefType>::getFromId(typename DrawableWrapper__<RefType>::IdType const &id) const
  {
    auto mapPart = d_map->find(id.second);
    return mapPart->second[id.first];
  }

  template<typename RefType>
  Drawable &DrawableWrapper__<RefType>::v_getFromId(typename DrawableWrapper__<RefType>::IdType const &id)
  {
    return getFromId(id);
  }

  template<typename RefType>
  Drawable const &DrawableWrapper__<RefType>::v_getFromId(typename DrawableWrapper__<RefType>::IdType const &id) const
  {
    return getFromId(id);
  }

  
  /* private functions */

  template <typename RefType>
  size_t DrawableWrapper__<RefType>::count() const
  {
    size_t count = 0;
    for(auto mapPart : *d_map)
      count += mapPart.second.size();

    return count;
  }
  
  /* constructors */

  template<typename RefType>
  DrawableWrapper__<RefType>::DrawableWrapper__(size_t gridSize)
      :
        DrawableWrapper__<Drawable>(gridSize),
        d_map(new std::unordered_map<Drawable::Key, std::vector<RefType>, std::hash<long>, std::equal_to<long>>())
  {
    if(gridSize != 0)
      instance() = *this;
  }

  /* regular functions */

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawableWrapper__<RefType>::add(bool changing, RefType const &object)
  {
    int xloc, zloc;
    xloc = object.coor().x / gridSize();
    zloc = object.coor().z / gridSize();
  
    if(changing)
      setChanged(true);

    std::vector<RefType> &list = (*d_map)[Drawable::Key(xloc, zloc)];

    std::pair<size_t, Drawable::Key> id(list.size(), Drawable::Key(xloc, zloc));

    //object.d_id = id;
    RefType tmp(object);
    tmp.d_id = id;

    list.push_back(tmp);
    return typename DrawableWrapper__<RefType>::iterator(id, this);
  }

  template<typename RefType>
  void DrawableWrapper__<RefType>::load(std::string const &strFilename)
  {
    setFilename(strFilename);
    /* open the file */
    std::ifstream file(filename().c_str());
    if(file.is_open() == false)
      throw std::runtime_error("Error opening " + filename());
    else
    {
      size_t numItems;
      file >> numItems;
      for(size_t idx = 0; idx != numItems; ++idx)
      {
        RefType ref;
        file >> ref;
        file.ignore();
        add(false, ref);
      }
      file.close();
    }
  }

  template<typename RefType>
  void DrawableWrapper__<RefType>::v_save()
  {
    if(changed() == false)
      return;

    std::ofstream file(filename().c_str(), std::fstream::trunc);
    if(file.is_open() == false)
    {
      file.close();
      throw std::runtime_error("Error opening " + filename());
    }

    file << count() << '\n';
    file.precision(0);
    file.setf(std::fstream::fixed);

    for(RefType &drawable : *this)
      file << drawable;

    file.close();
  }

  template<typename RefType>
  void DrawableWrapper__<RefType>::v_reset()
  {
    if(changed() == false)
      return;

    d_map->clear();
    load(filename());
  }

  template<typename RefType>
  void DrawableWrapper__<RefType>::v_draw(DrawState const &state, DrawableWrapper__<Drawable>::iterator &objSelect)
  {
    bool found = false;

    for(DrawState const &drawState : RefType().drawStates())
    {
      if(drawState == state)
      {
        found = true;
        break;
      }
    }

    if(found == false)
      return;

    for(auto mapPart : *d_map)
    {

      for(RefType &drawable : mapPart.second)
      {
        /* The drawing */
        if(drawable.id().first == objSelect.id().first && drawable.id().second == objSelect.id().second)
          Shader::active().send(0, "in_lighting");

        drawable.draw(state);

        if(drawable.id().first == objSelect.id().first && drawable.id().second == objSelect.id().second)
          Shader::active().send(1, "in_lighting");
        /* ... */
      }
    }
  }

  template<typename RefType>
  void DrawableWrapper__<RefType>::v_del(DrawableWrapper__<Drawable>::iterator &object)
  {
    setChanged(true);
    object->d_id = end().id();
  }

  template<typename RefType>
  DrawableWrapper__<Drawable>::iterator DrawableWrapper__<RefType>::v_find(float x, float z)
  {
    return DrawableWrapper__<Drawable>::iterator(find(x, z).id(), this);
  }

  template<typename RefType>
  typename DrawableWrapper__<RefType>::iterator DrawableWrapper__<RefType>::find(float x, float z)
  {


    int xloc, zloc;
    xloc = x / gridSize();
    zloc = z / gridSize();

    auto mapPart = d_map->find(Drawable::Key(xloc, zloc));
    if(mapPart == d_map->end())
      return end();

    for(RefType &drawable : mapPart->second)
    {
      glm::vec3 coor = drawable.coor();

      if((coor.x - x) * (coor.x - x) + (coor.z - z) * (coor.z - z) < drawable.d_radius)
        return typename DrawableWrapper__<RefType>::iterator(drawable.d_id, this);
    }
    return end();
  }
}
