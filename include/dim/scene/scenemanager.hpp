//      scenemanager.hpp
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

#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "dim/scene/texturemanager.hpp"
#include "dim/scene/scene.hpp"

namespace dim
{
  /**
   * A class to handle resources from disk. This class makes it possible to
   * request a scene as filename and the class will then keep a copy to
   * make sure the scene is not loaded twice
   */
  class SceneManager
  {
      std::unordered_map<std::string, Scene> d_sceneMap;

    public:
      /**
       * Either loads a Scene or returns the copy from an earlier load
       */
      Scene &request(std::string const &filename);
      /**
       * Either loads a Scene or returns the copy from an earlier load. If
       * any textures are loaded by the Scene object they are stored in the
       * TextureManager
       */
      Scene &request(std::string const &filename, TextureManager &resources);

      void remove(std::string const &filename); ///< Removes the stored loaded copy
  };
}

#endif
