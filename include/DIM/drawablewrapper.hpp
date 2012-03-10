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

#include "DIM/drawable.hpp"
#include "DIM/base_iterator__.hpp"
#include "DIM/onepair.hpp"

namespace dim
{
  class Drawable;

  template <typename... RefTypes>
  class DrawableWrapper__;

  template <>
  class DrawableWrapper__<Drawable>
  {
      std::shared_ptr<bool> d_changed;
      std::string d_filename;
      size_t d_gridSize;
    public:
      virtual ~DrawableWrapper__();

      DrawableWrapper__(size_t d_gridSize);

      // typedefs
      typedef std::pair<size_t, Drawable::Key> IdType;

      typedef base_iterator__<Drawable, DrawableWrapper__<Drawable>, IdType>  iterator;
      typedef base_iterator__<Drawable const, DrawableWrapper__<Drawable> const, IdType> const_iterator;
      // typedefs

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;

      void save();
      void reset();
      void draw(DrawState const &state, iterator &objSelect);
      iterator find(float x, float z);
      void del(iterator &object);

      std::pair<size_t, Drawable::Key> nextId(std::pair<size_t, Drawable::Key> const &id) const;
      //std::pair<size_t, Drawable::Key> const &previousId(std::pair<size_t, Drawable::Key> const &id) const;
      Drawable &getFromId(std::pair<size_t, Drawable::Key> const &id);
      Drawable const &getFromId(std::pair<size_t, Drawable::Key> const &id) const;

      size_t gridSize() const;
    private:
      virtual void v_save() = 0;
      virtual void v_reset() = 0;
      virtual void v_draw(DrawState const &state, iterator &objSelect) = 0;
      virtual iterator v_find(float x, float z) = 0;
      virtual void v_del(iterator &object) = 0;

      virtual IdType v_nextId(IdType const &id) const = 0;
      //virtual IdType const &v_previousId(IdType const &id) const = 0;
      virtual Drawable &v_getFromId(IdType const &id) = 0;
      virtual Drawable const &v_getFromId(IdType const &id) const = 0;

      virtual iterator v_begin() = 0;
      virtual iterator v_end() = 0;
      virtual const_iterator v_begin() const = 0;
      virtual const_iterator v_end() const = 0;

    protected:
      std::string const &filename() const;
      void setFilename(std::string const &filename);

      bool changed() const;
      void setChanged(bool changed);
  };

  template<typename RefType>
  class DrawableWrapper__<RefType> : public DrawableWrapper__<Drawable>
  {
      //static DrawableWrapper__<RefType> s_instance;
      std::shared_ptr<std::unordered_map<Drawable::Key, std::vector<RefType>, std::hash<long>, std::equal_to<long>>> d_map;

    public:
      static DrawableWrapper__<RefType> &instance();

      // typedefs
      typedef std::pair<size_t, Drawable::Key> IdType;

      typedef base_iterator__<RefType, DrawableWrapper__<RefType>, IdType> iterator;
      typedef base_iterator__<RefType const, DrawableWrapper__<RefType> const, IdType> const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;
      // typedefs

      DrawableWrapper__(size_t gridSize);

      iterator add(bool changing, RefType const &object);
      void load(std::string const &strFilename);
      iterator find(float x, float z);
      RefType &getFromId(IdType const &id);
      RefType const &getFromId(IdType const &id) const;

    private:
      size_t count() const;

      virtual void v_save();
      virtual void v_reset();
      virtual void v_draw(DrawState const &state, DrawableWrapper__<Drawable>::iterator &objSelect);
      virtual DrawableWrapper__<Drawable>::iterator v_find(float x, float z);
      virtual void v_del(DrawableWrapper__<Drawable>::iterator &object);

      virtual IdType v_nextId(IdType const &id) const;
      //virtual IdType const &v_previousId(IdType const &id) const;
      virtual Drawable &v_getFromId(IdType const &id);
      virtual Drawable const &v_getFromId(IdType const &id) const;

      DrawableWrapper__<Drawable>::iterator v_begin();
      DrawableWrapper__<Drawable>::iterator v_end();
      DrawableWrapper__<Drawable>::const_iterator v_begin() const;
      DrawableWrapper__<Drawable>::const_iterator v_end() const;
  };
  

}
#include "drawablewrapperimplement.hpp"
  
#endif
