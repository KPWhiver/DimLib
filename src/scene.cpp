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

#include "dim/scene.hpp"
#include <algorithm>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace std;

namespace dim
{
  DrawState::DrawState(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures)
  :
      d_mesh(mesh),
      d_textures(textures),
      d_culling(true)
  {
    sort(d_textures.begin(), d_textures.end(), [](pair<Texture<GLubyte>, string> const &lhs, pair<Texture<GLubyte>, string> const &rhs)
    {
      return lhs.first.id() < rhs.first.id();
    });
  }

  bool DrawState::culling() const
  {
    return d_culling;
  }

  void DrawState::setCulling(bool culling)
  {
    d_culling = culling;
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

  Mesh const &DrawState::mesh() const
  {
    return d_mesh;
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

    if(d_culling != other.d_culling)
      return false;

    return true;
  }

  bool DrawState::operator<(DrawState const &other) const
  {
    size_t minSize = min(d_textures.size(), other.d_textures.size());

    for(size_t idx = 0; idx != minSize; ++idx)
    {
      if(d_textures[idx].first.id() < other.d_textures[idx].first.id())
        return true;
      if(not d_textures[idx].first.id() == other.d_textures[idx].first.id())
        return false;
    }

    if(d_textures.size() < other.d_textures.size())
      return true;
    if(not d_textures.size() == other.d_textures.size())
      return false;

    if(d_mesh.id() < other.d_mesh.id())
      return true;
    if(not d_mesh.id() == other.d_mesh.id())
      return false;

    if(d_culling < other.d_culling)
      return true;

    return false;
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
    void addAttributeToBuffer(Attribute const &attrib, GLfloat *array, size_t offset, aiVector3D const &vector)
    {
      array[offset] = vector.x;

      if(attrib.format() > Attribute::vec1)
        array[offset + 1] = vector.y;

      if(attrib.format() > Attribute::vec2)
        array[offset + 2] = vector.z;
    }

    Mesh loadMesh(aiScene const &scene, size_t varNumOfElements, size_t mesh, Attribute const &vertex, Attribute const &normal, Attribute const &texCoord,
                  Attribute const &binormal, Attribute const &tangent)
    {
      vector<Attribute> attributes;
      attributes.push_back(vertex);
      if(normal.id() != Attribute::unknown)
        attributes.push_back(normal);
      if(texCoord.id() != Attribute::unknown)
        attributes.push_back(texCoord);
      if(binormal.id() != Attribute::unknown)
        attributes.push_back(binormal);
      if(tangent.id() != Attribute::unknown)
        attributes.push_back(tangent);

      // allocate buffer
      size_t numOfVertices = scene.mMeshes[mesh]->mNumVertices;

      GLfloat *array = new GLfloat[numOfVertices * varNumOfElements];

      // fill buffer
      for(size_t vert = 0; vert != numOfVertices; ++vert)
      {
        uint arrayIdxOffset = 0;
        size_t arrayIdxStart = vert * varNumOfElements;

        addAttributeToBuffer(vertex, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mVertices[vert]);
        arrayIdxOffset += vertex.size();

        if(normal.id() != Attribute::unknown)
        {
          addAttributeToBuffer(normal, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mNormals[vert]);
          arrayIdxOffset += normal.size();
        }

        if(texCoord.id() != Attribute::unknown)
        {
          addAttributeToBuffer(texCoord, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mTextureCoords[0][vert]);
          arrayIdxOffset += texCoord.size();
        }

        if(binormal.id() != Attribute::unknown)
        {
          addAttributeToBuffer(binormal, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mBitangents[vert]);
          arrayIdxOffset += binormal.size();
        }

        if(tangent.id() != Attribute::unknown)
        {
          addAttributeToBuffer(tangent, array, arrayIdxStart + arrayIdxOffset, scene.mMeshes[mesh]->mTangents[vert]);
          arrayIdxOffset += tangent.size();
        }
      }

      Mesh model(array, attributes, numOfVertices, Mesh::triangle, Mesh::interleaved);

      delete[] array;

      GLushort *indexArray = new GLushort[scene.mMeshes[mesh]->mNumFaces * 3];

      for(size_t idx = 0; idx != scene.mMeshes[mesh]->mNumFaces; ++idx)
      {
        indexArray[0 + idx * 3] = scene.mMeshes[mesh]->mFaces[idx].mIndices[0];
        indexArray[0 + idx * 3 + 1] = scene.mMeshes[mesh]->mFaces[idx].mIndices[1];
        indexArray[0 + idx * 3 + 2] = scene.mMeshes[mesh]->mFaces[idx].mIndices[2];
      }

      model.addElementBuffer(indexArray, scene.mMeshes[mesh]->mNumFaces);

      delete[] indexArray;

      return model;
    }
  }

  Scene::Scene(std::string const &filename, Attribute const &vertex, Attribute const &normal, Attribute const &texCoord,
               Attribute const &binormal, Attribute const &tangent)
  {
    // set flags
    uint flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;

    if(vertex.id() == Attribute::unknown)
    {
      log(__FILE__, __LINE__, LogType::error, "No vertex array specified while loading " + filename);
      return;
    }

    if(vertex.format() > Attribute::vec3 || normal.format() > Attribute::vec3 || texCoord.format() > Attribute::vec3 ||
       binormal.format() > Attribute::vec3 || tangent.format() > Attribute::vec3)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't extract more than 3 coordinates from a model file");
      return;
    }

    uint varNumOfElements = vertex.size();

    if(normal.id() != Attribute::unknown)
    {
      flags |= aiProcess_GenSmoothNormals;
      varNumOfElements += normal.size();
    }
    if(binormal.id() != Attribute::unknown)
    {
      flags |= aiProcess_CalcTangentSpace;
      varNumOfElements += binormal.size();
    }
    if(tangent.id() != Attribute::unknown)
    {
      flags |= aiProcess_CalcTangentSpace;
      varNumOfElements += tangent.size();
    }

    // load scene
    Assimp::Importer importer;
    aiScene const *scene = importer.ReadFile(filename, flags);

    if(!scene)
    {
      log(__FILE__, __LINE__, LogType::error, importer.GetErrorString());
      return;
    }

    if(texCoord.id() != Attribute::unknown && scene->mMeshes[0]->mTextureCoords[0] == 0)
    {
      log(filename, 0, LogType::error, "No texture coordinates present");
      return;
    }
    else
      varNumOfElements += texCoord.size();

    // load meshes
    for(size_t mesh = 0; mesh != scene->mNumMeshes; ++mesh)
      d_states.push_back(DrawState(loadMesh(*scene, varNumOfElements, mesh, vertex, normal, texCoord, binormal, tangent), {}));

    vector<vector<pair<Texture<GLubyte>, string>>> textures(scene->mNumMaterials);
    string baseName("in_texture");

    for(size_t material = 0; material != scene->mNumMaterials; ++material)
    {
      size_t texIdx = 0;
      aiString relativePath;
      aiReturn texFound;

      while(true)
      {
        texFound = scene->mMaterials[material]->GetTexture(aiTextureType_DIFFUSE, texIdx, &relativePath);
        if(texFound != AI_SUCCESS)
          break;

        string path = filename.substr(0, filename.find_last_of('/') + 1) + relativePath.data;

        textures[material].push_back(make_pair(Texture<GLubyte>(path, Filtering::trilinear, false), baseName + static_cast<char>('0' + texIdx)));
        ++texIdx;
      }
    }

    for(size_t mesh = 0; mesh != scene->mNumMeshes; ++mesh)
      d_states[mesh].setTextures(textures[scene->mMeshes[mesh]->mMaterialIndex]);

    sort(d_states.begin(), d_states.end());
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
    size_t minSize = min(size(), other.size());

    for(size_t idx = 0; idx != minSize; ++idx)
    {
      if(d_states[idx] < other.d_states[idx])
        return true;
      if(not (d_states[idx] == other.d_states[idx]))
        return false;
    }

    if(size() < other.size())
      return true;
    if(size() != other.size())
      return false;

    return false;
  }
}
