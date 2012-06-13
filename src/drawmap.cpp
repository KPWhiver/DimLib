//      drawmap.cpp
//      
//      Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <fstream>

#include "DIM/drawmap.hpp"
#include "DIM/shader.hpp"

#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace glm;

namespace dim
{

  /* constructors */
  
  DrawMap::DrawMap(size_t gridSize)
      : d_gridSize(gridSize), d_objSelect(end())
  {
  }

  DrawMap::~DrawMap()
  {
    for(size_t idx = 0; idx != d_drawableWrappers.size(); ++idx)
      delete d_drawableWrappers[idx];
  }

  DrawMap::iterator DrawMap::begin()
  {
    for(size_t idx = 0; idx != d_drawableWrappers.size(); ++idx)
    {
      DrawableWrapper__<Drawable>::iterator iter = d_drawableWrappers[idx]->begin();

      if(iter != d_drawableWrappers[idx]->end())
        return iterator(make_pair(idx, iter), this);

    }

    return end();
  }

  DrawMap::iterator DrawMap::end()
  {
    return iterator(
               make_pair(
                   0,
                   DrawableWrapper__<Drawable>::iterator(
                       DrawableWrapper__<Drawable>::IdType(std::numeric_limits<size_t>::max(), Drawable::Key())
                       , 0))
               , 0);
  }

  DrawMap::const_iterator DrawMap::begin() const
  {
    for(size_t idx = 0; idx != d_drawableWrappers.size(); ++idx)
    {
      DrawableWrapper__<Drawable>::iterator iter = d_drawableWrappers[idx]->begin();

      if(iter != d_drawableWrappers[idx]->end())
        return const_iterator(make_pair(idx, iter), this);
    }

    return end();
  }

  DrawMap::const_iterator DrawMap::end() const
  {
    return const_iterator(
               make_pair(
                   0,
                   DrawableWrapper__<Drawable>::iterator(
                       DrawableWrapper__<Drawable>::IdType(std::numeric_limits<size_t>::max(), Drawable::Key())
                       , 0))
               , 0);
  }

  void DrawMap::mark(DrawMap::iterator const &object)
  {
    d_objSelect = object;
  }

  void DrawMap::add(DrawState const &state)
  {
    d_drawStateList.insert(state);
  }

  void DrawMap::draw(DrawMap::DrawMode mode)
  {
    for(DrawState const &state: d_drawStateList)
    {
      if(mode == DrawMap::normal)
        state.shader().begin();
      else
      {
        glCullFace(GL_FRONT);
	      glEnable(GL_POLYGON_OFFSET_FILL);
	      glPolygonOffset(1.1f, 4.0f);
      }
      
      state.mesh().bind();
      if(state.culling() == false)
        glDisable(GL_CULL_FACE);

      for(size_t tex = 0; tex != state.textures().size(); ++tex)
        state.textures()[tex].first.send(tex, state.textures()[tex].second);

      //TODO save the drawables at the states

      for(DrawableWrapper__<Drawable> *wrapper: d_drawableWrappers)
        wrapper->draw(state, d_objSelect.id().second);
      
      if(mode == DrawMap::shadow)
      {
        glCullFace(GL_BACK);
	      glDisable(GL_POLYGON_OFFSET_FILL);
      }

      state.mesh().unbind();
      if(state.culling() == false)
        glEnable(GL_CULL_FACE);
    }
  }

  void DrawMap::del(DrawMap::iterator object)
  {
    //size_t id = object->id();
    //if(d_drawableWrappers.size() <= id%maxTypes)
    //  return;

    object.id().second.container()->del(object.id().second);

    //d_drawableWrappers[id%maxTypes]->del(object, id/maxTypes);

    //Drawable *oldId = d_drawableWrappers[id%maxTypes]->get(object, id/maxTypes);

    //if(oldId == 0)
    //  return;

    //oldId->d_id = 0;//TODO something
  }

  DrawMap::iterator DrawMap::find(float x, float z)
  {
    for(size_t idx = 0; idx != d_drawableWrappers.size(); ++idx)
    {
      auto iter = d_drawableWrappers[idx]->find(x, z);
      if(iter != d_drawableWrappers[idx]->end())
        return iterator(make_pair(idx, iter), this);
    }

    return end();
  }

  void DrawMap::save()
  {
    for(DrawableWrapper__<Drawable> *wrapper: d_drawableWrappers)
      wrapper->save();
  }

  void DrawMap::reset()
  {
    for(DrawableWrapper__<Drawable> *wrapper: d_drawableWrappers)
      wrapper->reset();
  }

  DrawMap::iterator DrawMap::get(float x, float z)
  {
    return find(x, z);
  }

  DrawMap::IdType DrawMap::nextId(IdType const &id) const
  {
    auto iter = id.second;

    for(size_t idx = id.first; idx != d_drawableWrappers.size(); ++idx)
    {
      ++iter;

      if(iter != d_drawableWrappers[idx]->end())
        return std::make_pair(idx, iter);

      if(idx + 1 != d_drawableWrappers.size())
        iter = d_drawableWrappers[idx + 1]->begin();
    }
    return end().id();
  }

  Drawable &DrawMap::getFromId(IdType const &id)
  {
    return *id.second;
  }

  Drawable const &DrawMap::getFromId(IdType const &id) const
  {
    return *id.second;
  }
}

