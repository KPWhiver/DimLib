// filedrawnode.hpp
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

#ifndef FILEDRAWNODE_HPP
#define FILEDRAWNODE_HPP

#include <string>
#include <unordered_map>

#include "dim/scene/nodebase.hpp"
#include "dim/scene/scene.hpp"
#include "dim/scene/texturemanager.hpp"
#include "dim/scene/scenemanager.hpp"
#include "dim/scene/shadermanager.hpp"
#include "dim/scene/bulletmanager.hpp"
#include "dim/core/shader.hpp"

#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace dim
{

  class FileDrawNode : public NodeBase
  {
      struct Object
      {
          std::string filename;
          std::vector<Shader> shaders;
          std::vector<Scene> scenes;
          //btRigidBody rigidBody;
      };

      static std::vector<Object> &objects();

      uint d_index;

      uint d_sceneIdx;

    public:
      FileDrawNode();
      FileDrawNode(std::string const &filename, glm::vec3 const &coor, glm::quat const &orient, glm::vec3 const &scale);

      static void setDefaultShaders(std::vector<Shader> const &shaders);

      Shader const &shader(size_t idx) const override;
      Scene const &scene() const override;

      uint sceneNumber() const;
      void setSceneNumber(uint index);
      uint numberOfScenes() const;

      btRigidBody *rigidBody() override;

      NodeBase *clone() const override;

      static void load(std::string const &filename, TextureManager &texRes, SceneManager &sceneRes, ShaderManager &shaderRes, BulletManager &bulletRes);

    private:
      static std::vector<Shader> &defaultShaders();

      void insert(std::ostream &out) const override;
      void extract(std::istream &in) override;
  };

}

#endif /* DRAWNODE_HPP_ */
