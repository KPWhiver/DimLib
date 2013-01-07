//      resourcemanager.cpp
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

#include "dim/scene/scenemanager.hpp"
#include "dim/scene/bulletmanager.hpp"
#include "dim/scene/shadermanager.hpp"

using namespace std;

namespace dim
{
  TextureManager::TextureManager(Filtering filter, Wrapping wrap)
    :
        d_defaultFilter(filter),
        d_defaultWrap(wrap)
  {
  }

  void TextureManager::setDefaultFiltering(Filtering filter)
  {
    d_defaultFilter = filter;
  }

  void TextureManager::setDefaultWrapping(Wrapping wrap)
  {
    d_defaultWrap = wrap;
  }

  btBulletWorldImporter &BulletManager::request(string const &filename)
  {
    if(d_bulletMap.find(filename) == d_bulletMap.end())
    {
      auto keyPair = d_bulletMap.emplace(filename, nullptr);
      if(keyPair.first->second.loadFile(filename.c_str()) == false)
      {
        log(__FILE__, __LINE__, LogType::error, "Failed to load bullet file: " + filename);
      }

      return keyPair.first->second;
    }

    return d_bulletMap.at(filename);
  }

  Texture<GLubyte> &TextureManager::request(string const &filename)
  {
    return request(filename, d_defaultFilter, false, d_defaultWrap);
  }

  Texture<GLubyte> &TextureManager::request(string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap)
  {
    if(d_textureMap.find(filename) == d_textureMap.end())
      return d_textureMap[filename] = Texture<GLubyte>(filename, filter, false, wrap);

    return d_textureMap[filename];
  }

  Shader &ShaderManager::request(string const &filename)
  {
    if(d_shaderMap.find(filename) == d_shaderMap.end())
    {
      auto keyPair = d_shaderMap.emplace(filename, filename);
      return keyPair.first->second;
    }

    return d_shaderMap.at(filename);
  }

  Scene &SceneManager::request(string const &filename, TextureManager &resources)
  {
    if(d_sceneMap.find(filename) == d_sceneMap.end())
      return d_sceneMap[filename] = Scene(filename, resources,
                                          {Attribute::vertex, Attribute::vec3},
                                          {Attribute::normal, Attribute::vec3},
                                          {Attribute::texCoord, Attribute::vec2});

    return d_sceneMap[filename];
  }

  Scene &SceneManager::request(string const &filename)
  {
    if(d_sceneMap.find(filename) == d_sceneMap.end())
      return d_sceneMap[filename] = Scene(filename,
                                          {Attribute::vertex, Attribute::vec3},
                                          {Attribute::normal, Attribute::vec3},
                                          {Attribute::texCoord, Attribute::vec2});

    return d_sceneMap[filename];
  }

  void BulletManager::remove(string const &filename)
  {
    auto iter = d_bulletMap.find(filename);

    if(iter == d_bulletMap.end())
    {
      log(__FILE__, __LINE__, LogType::warning, "Couldn't delete " + filename + " because it has never been loaded");
      return;
    }

    d_bulletMap.erase(iter);
  }

  void TextureManager::remove(string const &filename)
  {
    auto iter = d_textureMap.find(filename);

    if(iter == d_textureMap.end())
    {
      log(__FILE__, __LINE__, LogType::warning, "Couldn't delete " + filename + " because it has never been loaded");
      return;
    }

    d_textureMap.erase(iter);
  }

  void ShaderManager::remove(string const &filename)
  {
    auto iter = d_shaderMap.find(filename);

    if(iter == d_shaderMap.end())
    {
      log(__FILE__, __LINE__, LogType::warning, "Couldn't delete " + filename + " because it has never been loaded");
      return;
    }

    d_shaderMap.erase(iter);
  }

  void SceneManager::remove(string const &filename)
  {
    auto iter = d_sceneMap.find(filename);

    if(iter == d_sceneMap.end())
    {
      log(__FILE__, __LINE__, LogType::warning, "Couldn't delete " + filename + " because it has never been loaded");
      return;
    }

    d_sceneMap.erase(iter);
  }
}


