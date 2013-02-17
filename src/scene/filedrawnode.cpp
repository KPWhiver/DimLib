// filedrawnode.cpp
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

#include "dim/scene/filedrawnode.hpp"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

namespace dim
{
  vector<FileDrawNode::Object> FileDrawNode::s_objects;

  FileDrawNode::FileDrawNode(string const &filename, glm::vec3 const &coor, glm::quat const &orient, glm::vec3 const &scale)
  :
      NodeBase(coor, orient, scale),
      d_index(0)
  {
    bool present = false;

    for(size_t idx = 0; idx != s_objects.size(); ++idx)
    {
      if(s_objects[idx].filename == filename)
      {
        d_index = idx;
        present = true;
        break;
      }
    }

    if(not present)
    {
      log(__FILE__, __LINE__, LogType::error, "The file " + filename + " needs to be loaded first with the static 'load' member");
      return;
    }
  }

  void FileDrawNode::load(string const &filename, TextureManager &texRes, SceneManager &sceneRes, ShaderManager &shaderRes, BulletManager &bulletRes)
  {
    for(size_t idx = 0; idx != s_objects.size(); ++idx)
    {
      // if it's already loaded we can stop
      if(s_objects[idx].filename == filename)
      {
        log(__FILE__, __LINE__, LogType::warning, "The file " + filename + " is already loaded");
        return;
      }
    }

    // the status the file reader is in
    enum Status
    {
      bullet,
      scene,
      shader,
      none,
    };
    Status status(none);

    // open the file
    ifstream file(filename);
    if(not file.is_open())
    {
      log(__FILE__, __LINE__, LogType::error, "Can't open file " + filename + " for reading");
      return;
    }

    // determine location of the file
    string directory;
    size_t locOfLastSlash = filename.find_last_of('/', string::npos);
    if(locOfLastSlash != string::npos)
      directory = filename.substr(0, locOfLastSlash + 1);

    string sceneFile;
    string shaderFile;
    string rigidBodyFile;

    // read the lines
    string line;
    while(getline(file, line))
    {
      if(line.find("bullet:") != string::npos)
        status = bullet;
      else if(line.find("scene:") != string::npos)
        status = scene;
      else if(line.find("shader:") != string::npos)
        status = shader;
      else
      {
        if(status == bullet)
        {
          rigidBodyFile = directory + line;
          status = none;
        }
        else if(status == scene)
        {
          sceneFile = directory + line;
          status = none;
        }
        else if(status == shader)
        {
          shaderFile = directory + line;
          status = none;
        }
      }
    }

    // get the bullet object
    //btBulletWorldImporter importer = bulletRes(rigidBodyFile);
    //btRigidBody rigidBody = importer.get

    s_objects.push_back({filename, shaderRes.request(shaderFile), sceneRes.request(sceneFile, texRes)});
  }

  Shader const &FileDrawNode::shader() const
  {
    return s_objects[d_index].shader;
  }
  
  Scene const &FileDrawNode::scene() const
  {
    return s_objects[d_index].scene;
  }

  btRigidBody *FileDrawNode::rigidBody()
  {
    return 0;
    //return &s_objects[d_index].rigidBody;
  }

  NodeBase *FileDrawNode::clone() const
  {
    return new FileDrawNode(*this);
  }
}
