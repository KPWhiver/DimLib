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
    // constructors
      virtual ~DrawableWrapper__();
      DrawableWrapper__(size_t d_gridSize);
      
      DrawableWrapper__(DrawableWrapper__ const &other);
      
    // static access
      void remove(DrawMap* key) const;
      void copy(DrawMap* source, DrawMap* dest) const;
      void move(DrawMap* source, DrawMap* dest) const;

    // typedefs
      typedef std::pair<size_t, Drawable::Key> IdType;

    // iterators
      typedef base_iterator__<Drawable, DrawableWrapper__<Drawable>, IdType>  iterator;
      typedef base_iterator__<Drawable const, DrawableWrapper__<Drawable> const, IdType> const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;
      
      std::pair<size_t, Drawable::Key> nextId(std::pair<size_t, Drawable::Key> const &id) const;
      //std::pair<size_t, Drawable::Key> const &previousId(std::pair<size_t, Drawable::Key> const &id) const;
      Drawable &getFromId(std::pair<size_t, Drawable::Key> const &id);
      Drawable const &getFromId(std::pair<size_t, Drawable::Key> const &id) const;

    // regular functions
      void save();
      void reset();
      void draw(DrawState const &state, iterator &objSelect);
      iterator find(float x, float z);
      void del(iterator &object);

      size_t gridSize() const;
      
    private:
    // static access
      virtual void v_remove(DrawMap* key) const = 0;
      virtual void v_copy(DrawMap* source, DrawMap* dest) const = 0;
      virtual void v_remove(DrawMap* source, DrawMap* dest) const = 0;
      virtual DrawableWrapper__<Drawable>* v_clone() const = 0;
    
    // iterators
      virtual IdType v_nextId(IdType const &id) const = 0;
      //virtual IdType const &v_previousId(IdType const &id) const = 0;
      virtual Drawable &v_getFromId(IdType const &id) = 0;
      virtual Drawable const &v_getFromId(IdType const &id) const = 0;

      virtual iterator v_begin() = 0;
      virtual iterator v_end() = 0;
      virtual const_iterator v_begin() const = 0;
      virtual const_iterator v_end() const = 0;
      
    // regular functions
      virtual void v_save() = 0;
      virtual void v_reset() = 0;
      virtual void v_draw(DrawState const &state, iterator &objSelect) = 0;
      virtual iterator v_find(float x, float z) = 0;
      virtual void v_del(iterator &object) = 0;

    protected:
    // private functions
      std::string const &filename() const;
      void setFilename(std::string const &filename);

      bool changed() const;
      void setChanged(bool changed);
  };

  template<typename RefType>
  class DrawableWrapper__<RefType> : public DrawableWrapper__<Drawable>
  {
      typedef std::unordered_map<Drawable::Key, std::vector<RefType>, std::hash<long>, std::equal_to<long>> Storage; 
      
      static std::unordered_map<DrawMap*, Storage> s_map;

    public:
    // constuctors
      DrawableWrapper__(size_t gridSize);
      
    // static access
      static DrawableWrapper__<RefType> &get(DrawMap* key);
      
      static void remove(DrawMap* key);
      static void copy(DrawMap* source, DrawMap* dest);
      static void move(DrawMap* source, DrawMap* dest);
      
      static bool isPresent(DrawMap* key);

    // typedefs
      typedef std::pair<size_t, Drawable::Key> IdType;

    // iterators
      typedef base_iterator__<RefType, DrawableWrapper__<RefType>, IdType> iterator;
      typedef base_iterator__<RefType const, DrawableWrapper__<RefType> const, IdType> const_iterator;

      iterator begin();
      iterator end();
      const_iterator begin() const;
      const_iterator end() const;
      
      RefType &getFromId(IdType const &id);
      RefType const &getFromId(IdType const &id) const;

    // regular functions
      iterator add(bool changing, RefType const &object);
      void load(std::string const &strFilename);
      iterator find(float x, float z);

    private:
    // static access
      virtual void v_remove(DrawMap* key) const;
      virtual void v_copy(DrawMap* source, DrawMap* dest) const;
      virtual void v_remove(DrawMap* source, DrawMap* dest) const;
      virtual DrawableWrapper__<Drawable>* v_clone() const;

    // regular functions
      virtual void v_save();
      virtual void v_reset();
      virtual void v_draw(DrawState const &state, DrawableWrapper__<Drawable>::iterator &objSelect);
      virtual DrawableWrapper__<Drawable>::iterator v_find(float x, float z);
      virtual void v_del(DrawableWrapper__<Drawable>::iterator &object);

    // iterators
      virtual IdType v_nextId(IdType const &id) const;
      //virtual IdType const &v_previousId(IdType const &id) const;
      virtual Drawable &v_getFromId(IdType const &id);
      virtual Drawable const &v_getFromId(IdType const &id) const;

      DrawableWrapper__<Drawable>::iterator v_begin();
      DrawableWrapper__<Drawable>::iterator v_end();
      DrawableWrapper__<Drawable>::const_iterator v_begin() const;
      DrawableWrapper__<Drawable>::const_iterator v_end() const;

    // private functions
      size_t count() const;
  };
  

}
#include "drawablewrapper.inl"
  
#endif
