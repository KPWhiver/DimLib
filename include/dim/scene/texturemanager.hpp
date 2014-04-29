//      texturemanager.hpp
//
//      Copyright 2013 Klaas Winter <klaaswinter@gmail.com>
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

#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <string>
#include <unordered_map>
#include "dim/core/texture.hpp"

namespace dim
{
  /**
   * A class to handle resources from disk. This class makes it possible to
   * request a texture as filename and the class will then keep a copy to
   * make sure the texture is not loaded twice
   */
  class TextureManager
  {
      std::unordered_map<std::string, Texture<GLubyte>> d_textureMap;

      Filtering d_defaultFilter;
      Wrapping d_defaultWrap;

    public:
      TextureManager(Filtering filter = Filtering::linear, Wrapping wrap = Wrapping::edgeClamp); ///< Default constructor

      /**
       * Sets the default wrapping used to create a texture when calling
       * loadTexture(std::string)
       */
      void setDefaultWrapping(Wrapping wrap);
      /**
       * Sets the default filtering used to create a texture when calling
       * loadTexture(std::string)
       */
      void setDefaultFiltering(Filtering filter);

      /**
       * Either loads a Texture with the given arguments or returns the copy
       * from an earlier load
       */
      Texture<GLubyte> &request(std::string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
      /**
       * Either loads a Texture or returns the copy from an earlier load
       */
      Texture<GLubyte> &request(std::string const &filename);

      void remove(std::string const &filename); ///< Removes the stored loaded copy
  };
}

#endif
