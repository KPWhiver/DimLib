// drawable.cpp
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

#include "dim/scene/scene.hpp"
#include "dim/core/shader.hpp"
#include <algorithm>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace std;
using namespace glm;

namespace dim
{
  DrawState::DrawState(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures)
  :
      d_mesh(mesh),
      d_textures(textures),
      d_ambient(1.0, 1.0, 1.0),
      d_diffuse(1.0, 1.0, 1.0),
      d_specular(1.0, 1.0, 1.0),
      d_shininess(0)
  {
    sort(d_textures.begin(), d_textures.end(), [](pair<Texture<GLubyte>, string> const &lhs, pair<Texture<GLubyte>, string> const &rhs)
    {
      return lhs.first.id() < rhs.first.id();
    });
  }

  vector<pair<Texture<GLubyte>, string>> const &DrawState::textures() const
  {
    return d_textures;
  }

  void DrawState::setTextures(vector<pair<Texture<GLubyte>, string>> const &param)
  {
    d_textures = param;
    sort(d_textures.begin(), d_textures.end(), [](pair<Texture<GLubyte>, string> const &lhs, pair<Texture<GLubyte>, string> const &rhs)
    {
      return lhs.first.id() < rhs.first.id();
    });
  }


  void DrawState::setMaterial(vec3 ambient, vec3 diffuse, vec3 specular, float shininess)
  {
    d_ambient = ambient;
    d_diffuse = diffuse;
    d_specular = specular;
    d_shininess = shininess;
  }

  Mesh const &DrawState::mesh() const
  {
    return d_mesh;
  }

  vec3 const &DrawState::ambientIntensity() const
  {
    return d_ambient;
  }

  vec3 const &DrawState::diffuseIntensity() const
  {
    return d_diffuse;
  }

  vec3 const &DrawState::specularIntensity() const
  {
    return d_specular;
  }

  float DrawState::shininess() const
  {
    return d_shininess;
  }

  bool DrawState::operator==(DrawState const &other) const
  {
    if(d_textures.size() != other.d_textures.size())
      return false;

    for(size_t idx = 0; idx != d_textures.size(); ++idx)
    {
      if(d_textures[idx].first.id() != other.d_textures[idx].first.id())
        return false;
    }

    if(d_mesh.id() != other.d_mesh.id())
      return false;

    return true;
  }

  bool DrawState::operator<(DrawState const &other) const
  {
    size_t minSize = std::min(d_textures.size(), other.d_textures.size());

    for(size_t idx = 0; idx != minSize; ++idx)
    {
      if(d_textures[idx].first.id() < other.d_textures[idx].first.id())
        return true;
      if(d_textures[idx].first.id() > other.d_textures[idx].first.id())
        return false;
    }

    if(d_textures.size() < other.d_textures.size())
      return true;
    if(d_textures.size() > other.d_textures.size())
      return false;

    if(d_mesh.id() < other.d_mesh.id())
      return true;
    if(d_mesh.id() > other.d_mesh.id())
      return false;

    if(d_shininess < other.d_shininess)
      return true;
    if(d_shininess > other.d_shininess)
      return false;

    return false;
  }

	void DrawState::draw() const
	{
		/*Shader const &shader = Shader::active();

		uint count = 0;
		for(auto const &texturePair : d_textures)
		{
			shader.set(texturePair.second, texturePair.first, count);
			++count;
		}*/

		//TODO: add the setting of all parameters (textures, materials)

		d_mesh.draw();


	}

	// Scene

  Scene::Scene(Mesh const &mesh, std::vector<pair<Texture<GLubyte>, string>> const &textures)
  {
    add(mesh, textures);
  }

  DrawState &Scene::operator[](size_t idx)
  {
    return d_states[idx];
  }

  DrawState const &Scene::operator[](size_t idx) const
  {
    return d_states[idx];
  }

  size_t Scene::size() const
  {
    return d_states.size();
  }

  void Scene::add(Mesh const &mesh, std::vector<pair<Texture<GLubyte>, string>> const &textures)
  {
    d_states.push_back(DrawState(mesh, textures));
    sort(d_states.begin(), d_states.end());
  }

  namespace
  {
    void addAttributeToBuffer(size_t numOfElements, vector<GLfloat> &array, size_t offset, aiVector3D const &vector)
    {
      array[offset] = vector.x;
      array[offset + 1] = vector.y;

      if(numOfElements > 2)
        array[offset + 2] = vector.z;
    }

    bool in(vector<Scene::Option> const &list, Scene::Option option)
    {
      for(Scene::Option item : list)
      {
        if(item == option)
          return true;
      }
      return false;
    }

    Mesh loadMesh(aiScene const &scene, std::vector<Scene::Option> const &options, size_t mesh, string const &filename)
    {
      vector<Attribute> attributes;
      attributes.push_back({Attribute::vertex, Attribute::vec3});

      if(in(options, Scene::texCoords3D) && scene.mMeshes[mesh]->mTextureCoords[0] == 0)
        throw log(filename, 0, LogType::error, "No texture coordinates present");

      uint numOfElements = 3;
      bool texCoords = false;
      bool normals = false;
      bool binormals = false;
      bool tangents = false;
      size_t numOfTexCoords = 0;

      if(scene.mMeshes[mesh]->mNormals != 0 && ! in(options, Scene::noNormals))
      {
        numOfElements += 3;
        normals = true;

        attributes.push_back({Attribute::normal, Attribute::vec3});
      }

      if(scene.mMeshes[mesh]->mTextureCoords[0] != 0 && ! in(options, Scene::noTexCoords))
      {
        if(in(options, Scene::texCoords3D))
        {
          numOfElements += 3;
          numOfTexCoords = 3;
          attributes.push_back({Attribute::texCoord, Attribute::vec3});
        }
        else
        {
          numOfElements += 2;
          numOfTexCoords = 2;
          attributes.push_back({Attribute::texCoord, Attribute::vec2});
        }

        texCoords = true;
      }



      if(scene.mMeshes[mesh]->mBitangents != 0 && in(options, Scene::generateBinormals))
      {
        numOfElements += 3;
        binormals = true;

        attributes.push_back({Attribute::binormal, Attribute::vec3});
      }

      if(scene.mMeshes[mesh]->mTangents != 0 && in(options, Scene::generateTangents))
      {
        numOfElements += 3;
        tangents = true;

        attributes.push_back({Attribute::tangent, Attribute::vec3});
      }

      // allocate buffer
      size_t numOfVertices = scene.mMeshes[mesh]->mNumVertices;

      vector<GLfloat> array(numOfVertices * numOfElements);

      // fill buffer
      for(size_t vert = 0; vert != numOfVertices; ++vert)
      {
        uint arrayIdxOffset = 0;
        size_t arrayIdxStart = vert * numOfElements;

        addAttributeToBuffer(3, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mVertices[vert]);
        arrayIdxOffset += 3;

        if(normals)
        {
          addAttributeToBuffer(3, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mNormals[vert]);
          arrayIdxOffset += 3;
        }

        if(texCoords)
        {
          addAttributeToBuffer(numOfTexCoords, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mTextureCoords[0][vert]);
          arrayIdxOffset += numOfTexCoords;
        }

        if(binormals)
        {
          addAttributeToBuffer(3, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mBitangents[vert]);
          arrayIdxOffset += 3;
        }

        if(tangents)
        {
          addAttributeToBuffer(3, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mTangents[vert]);
          arrayIdxOffset += 3;
        }
      }

      Mesh model(array.data(), attributes, numOfVertices, Mesh::triangle, Mesh::interleaved);

      vector<GLushort> indexArray(scene.mMeshes[mesh]->mNumFaces * 3);

      for(size_t idx = 0; idx != scene.mMeshes[mesh]->mNumFaces; ++idx)
      {
        indexArray[0 + idx * 3] = scene.mMeshes[mesh]->mFaces[idx].mIndices[0];
        indexArray[0 + idx * 3 + 1] = scene.mMeshes[mesh]->mFaces[idx].mIndices[1];
        indexArray[0 + idx * 3 + 2] = scene.mMeshes[mesh]->mFaces[idx].mIndices[2];
      }

      model.addElementBuffer(indexArray.data(), scene.mMeshes[mesh]->mNumFaces);

      return model;
    }
  }

  namespace
  {
    TextureManager stdManager(Filtering::trilinear);
  }

  Scene::Scene(std::string const &filename, std::vector<Option> list)
  :
      Scene(filename, stdManager, list)
  {
  }

  Scene::Scene(std::string const &filename, TextureManager &resources, std::vector<Option> options)
  {
    // set flags
    uint flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;

    if(in(options, generateNormals))
      flags |= aiProcess_GenSmoothNormals;

    if(in(options, generateTangents))
    {
      flags |= aiProcess_GenSmoothNormals;
      flags |= aiProcess_CalcTangentSpace;
    }
    if(in(options, generateBinormals))
    {
      flags |= aiProcess_GenSmoothNormals;
      flags |= aiProcess_CalcTangentSpace;
    }

    // load scene
    Assimp::Importer importer;
    aiScene const *scene = importer.ReadFile(filename, flags);

    if(!scene)
      throw log(__FILE__, __LINE__, LogType::error, importer.GetErrorString());

    if(in(options, texCoords3D) && scene->mMeshes[0]->mTextureCoords[0] == 0)
      throw log(filename, 0, LogType::error, "No texture coordinates present");

    // load meshes
    for(size_t mesh = 0; mesh != scene->mNumMeshes; ++mesh)
      d_states.push_back(DrawState(loadMesh(*scene, options, mesh, filename), {}));

    vector<vector<pair<Texture<GLubyte>, string>>> textures(scene->mNumMaterials);
    vector<aiColor3D> ambientColors(scene->mNumMaterials, aiColor3D(1.0, 1.0, 1.0));
    vector<aiColor3D> diffuseColors(scene->mNumMaterials, aiColor3D(1.0, 1.0, 1.0));
    vector<aiColor3D> specularColors(scene->mNumMaterials, aiColor3D(1.0, 1.0, 1.0));
    vector<float> shininess(scene->mNumMaterials, 0);
    vector<float> specularIntensity(scene->mNumMaterials, 1);
    string baseName("in_texture");

    for(size_t material = 0; material != scene->mNumMaterials; ++material)
    {
      aiString relativePath;
      aiReturn texFound;

      // load textures
      for(size_t texture = 0; texture != scene->mMaterials[material]->GetTextureCount(aiTextureType_DIFFUSE); ++texture)
      {
        texFound = scene->mMaterials[material]->GetTexture(aiTextureType_DIFFUSE, texture, &relativePath);
        if(texFound != AI_SUCCESS)
        {
          log(__FILE__, __LINE__, LogType::warning, "This should not happen");
          break;
        }

        string path = filename.substr(0, filename.find_last_of('/') + 1) + relativePath.data;

        textures[material].emplace_back(resources.request(path), baseName + static_cast<char>('0' + texture));
      }
      // load light material settings
      scene->mMaterials[material]->Get(AI_MATKEY_COLOR_AMBIENT, ambientColors[material]);
      scene->mMaterials[material]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColors[material]);
      scene->mMaterials[material]->Get(AI_MATKEY_COLOR_SPECULAR, specularColors[material]);
      scene->mMaterials[material]->Get(AI_MATKEY_SHININESS, shininess[material]);
      scene->mMaterials[material]->Get(AI_MATKEY_SHININESS_STRENGTH, specularIntensity[material]);
    }

    // set textures and materials inside object
    for(size_t mesh = 0; mesh != scene->mNumMeshes; ++mesh)
    {
      uint materialIdx = scene->mMeshes[mesh]->mMaterialIndex;

      d_states[mesh].setTextures(textures[materialIdx]);
      d_states[mesh].setMaterial(vec3(ambientColors[materialIdx].r, ambientColors[materialIdx].g, ambientColors[materialIdx].b),
                                 vec3(diffuseColors[materialIdx].r, diffuseColors[materialIdx].g, diffuseColors[materialIdx].b),
                                 vec3(specularColors[materialIdx].r, specularColors[materialIdx].g, specularColors[materialIdx].b) * specularIntensity[materialIdx],
                                 shininess[materialIdx]);
    }

    sort(d_states.begin(), d_states.end());
  }

  void Scene::draw() const
  {
    for(DrawState const &drawState : d_states)
      drawState.draw();
  }

  bool Scene::operator==(Scene const &other) const
  {
    if(size() != other.size())
      return false;

    for(size_t idx = 0; idx != size(); ++idx)
    {
      if(not (d_states[idx] == other.d_states[idx]))
        return false;
    }

    return true;
  }

  bool Scene::operator<(Scene const &other) const
  {
    size_t minSize = std::min(size(), other.size());

    for(size_t idx = 0; idx != minSize; ++idx)
    {
      if(d_states[idx] < other.d_states[idx])
        return true;
      if(not (d_states[idx] == other.d_states[idx]))
        return false;
    }

    if(size() < other.size())
      return true;
    if(size() > other.size())
      return false;

    return false;
  }
}
