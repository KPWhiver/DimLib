// drawstate.hpp
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

#ifndef DRAWSTATE_HPP
#define DRAWSTATE_HPP

#include <vector>

#include "dim/core/mesh.hpp"
#include "dim/core/texture.hpp"
#include "dim/core/shader.hpp"
#include "dim/scene/texturemanager.hpp"

namespace dim
{

class DrawState
{
    friend class Scene;

    Mesh d_mesh;
    std::vector<std::pair<Texture<GLubyte>, std::string>> d_textures;
    glm::vec3 d_ambient;
    glm::vec3 d_diffuse;
    glm::vec3 d_specular;
    float d_shininess;

    DrawState(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures);
  
  public:
    std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures() const;
    Mesh const &mesh() const;
    
    void setTextures(std::vector<std::pair<Texture<GLubyte>, std::string>> const &param);
    void setMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

    glm::vec3 const &ambientIntensity() const;
    glm::vec3 const &diffuseIntensity() const;
    glm::vec3 const &specularIntensity() const;
    float shininess() const;

    bool operator==(DrawState const &other) const;
    bool operator<(DrawState const &other) const;

    void draw() const;
};

class Scene
{
  std::vector<DrawState> d_states;

public:
  enum Option : int
  {
    noNormals,
    noTexCoords,
    generateNormals,
    regenerateNormals,
    //generateTexCoords,
    texCoords3D,
    generateTangents,
    generateBinormals,
    load1BoneWeights,
    load2BoneWeights,
    load4BoneWeights,
    //load8BoneWeights
  };

  Scene() = default;

  Scene(std::string const &filename, std::vector<Option> list = {});
  Scene(std::string const &filename, TextureManager &resources, std::vector<Option> options = {});

	Scene(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures = {});
	void add(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures = {});

  void draw() const;

  DrawState &operator[](size_t idx);
  DrawState const &operator[](size_t idx) const;

  size_t size() const;

  bool operator==(Scene const &other) const;
  bool operator<(Scene const &other) const;

  static std::vector<GLfloat> loadPointData(std::string const &filename, std::vector<Option> list = {});
  static std::pair<std::vector<GLfloat>, Bone> loadPointDataAndBones(std::string const &filename, std::vector<Option> list = {});
};
}

#endif /* DRAWSTATE_HPP */
