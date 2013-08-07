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
#include <yaml-cpp/yaml.h>

using namespace glm;
using namespace std;

namespace dim
{
  vector<FileDrawNode::Object> &FileDrawNode::objects()
  {
    static vector<FileDrawNode::Object> list;
    return list;
  }

  Shader &FileDrawNode::defaultShader()
  {
    static Shader shader(Shader::defaultShader());
    return shader;
  }

  void FileDrawNode::setDefaultShader(Shader const &shader)
  {
    defaultShader() = shader;
  }

  FileDrawNode::FileDrawNode()
    :
        d_index(0),
        d_sceneIdx(0)
  {
  }


  FileDrawNode::FileDrawNode(string const &filename, glm::vec3 const &coor, glm::quat const &orient, glm::vec3 const &scale)
  :
      NodeBase(coor, orient, scale),
      d_index(0),
      d_sceneIdx(0)
  {
    bool present = false;

    for(size_t idx = 0; idx != objects().size(); ++idx)
    {
      if(objects()[idx].filename == filename)
      {
        d_index = idx;
        d_sceneIdx = rand() % objects()[idx].scenes.size();
        present = true;
        break;
      }
    }

    if(not present)
      throw log(__FILE__, __LINE__, LogType::error, "The file " + filename + " needs to be loaded first with the static 'load' member");
  }

  vector<Scene> parseScenes(YAML::Node const *node, string const &filename, string const &directory, TextureManager &texRes, SceneManager &sceneRes, BulletManager &bulletRes)
  {
    if(node == 0)
      throw log(__FILE__, __LINE__, LogType::error, "The file " + filename + " does not contain a scenes section");

    // Read and parse the scene objects
    //if(node->Type() == YAML::NodeType::Sequence)
    //  throw log(__FILE__, __LINE__, LogType::error, "Expected Scenes in " + filename + " to be a sequence");

    vector<Scene> scenes;
    for(YAML::Iterator it = node->begin(); it != node->end(); ++it)
    {
      string sceneFile;
      (*it)["modelFile"] >> sceneFile;
      scenes.push_back(sceneRes.request(directory + sceneFile, texRes));
    }

    if(scenes.size() == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Expected at least one scene in " + filename);

    return scenes;
  }

  Shader &parseShader(YAML::Node const *node, Shader &defaultShader, string const &directory, ShaderManager &shaderRes)
  {
    if(node == 0)
      return defaultShader;
    else
    {
      string shaderFile;
      (*node)["file"] >> shaderFile;
      return shaderRes.request(directory + shaderFile);
    }
  }

  void FileDrawNode::load(string const &filename, TextureManager &texRes, SceneManager &sceneRes, ShaderManager &shaderRes, BulletManager &bulletRes)
  {
    for(size_t idx = 0; idx != objects().size(); ++idx)
    {
      // if it's already loaded we can stop
      if(objects()[idx].filename == filename)
      {
        log(__FILE__, __LINE__, LogType::warning, "The file " + filename + " is already loaded");
        return;
      }
    }

    // open the file
    ifstream file(filename);
    if(not file.is_open())
    {
      throw log(__FILE__, __LINE__, LogType::error, "Can't open file " + filename + " for reading");
      return;
    }

    try
    {
      // determine location of the file
      string directory;
      size_t locOfLastSlash = filename.find_last_of('/', string::npos);
      if(locOfLastSlash != string::npos)
        directory = filename.substr(0, locOfLastSlash + 1);

      YAML::Parser parser(file);
      YAML::Node document;
      parser.GetNextDocument(document);

      vector<Scene> scenes = parseScenes(document.FindValue("Scenes"), filename, directory, texRes, sceneRes, bulletRes);
      Shader &shader = parseShader(document.FindValue("Shader"), defaultShader(), directory, shaderRes);

      objects().push_back({filename, shader, scenes});
    }
    catch(exception &except)
    {
      throw log(__FILE__, __LINE__, LogType::error, except.what());
    }
  }

  uint FileDrawNode::sceneNumber() const
  {
    return d_sceneIdx;
  }

  void FileDrawNode::setSceneNumber(uint index)
  {
    d_sceneIdx = index;
  }

  uint FileDrawNode::numberOfScenes() const
  {
    return objects()[d_index].scenes.size();
  }

  Shader const &FileDrawNode::shader(size_t idx) const
  {
    return objects()[d_index].shader;
  }
  
  Scene const &FileDrawNode::scene() const
  {
    return objects()[d_index].scenes[d_sceneIdx];
  }

  btRigidBody *FileDrawNode::rigidBody()
  {
    return 0;
    //return &objects()[d_index].rigidBody;
  }

  NodeBase *FileDrawNode::clone() const
  {
    return new FileDrawNode(*this);
  }

  void FileDrawNode::insert(std::ostream &out) const
  {
    out << location().x << ' ' << location().y << ' ' << location().z << ' '
        << orientation().x << ' ' << orientation().y << ' ' << orientation().z << ' ' << orientation().w << ' ' << d_sceneIdx << ' ' << objects()[d_index].filename << '\n';
  }

  void FileDrawNode::extract(std::istream &in)
  {
    string filename;
    vec3 l_coor;
    quat l_rotation;

    in >> l_coor.x >> l_coor.y >> l_coor.z >> l_rotation.x >> l_rotation.y >> l_rotation.z >> l_rotation.w >> d_sceneIdx >> filename;
    in.ignore();

    if(not in)
      return;

    bool present = false;

    for(size_t idx = 0; idx != objects().size(); ++idx)
    {
      if(objects()[idx].filename == filename)
      {
        d_index = idx;
        present = true;
        break;
      }
    }

    if(not present)
      throw log(__FILE__, __LINE__, LogType::error, "The file " + filename + " needs to be loaded first with the static 'load' member");

    setLocation(l_coor);
    setOrientation(l_rotation);
  }
}
