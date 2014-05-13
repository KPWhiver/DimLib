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
    bool in(vector<Scene::Option> const &list, Scene::Option option)
    {
      for(Scene::Option item : list)
      {
        if(item == option)
          return true;
      }
      return false;
    }

    void addTexCoordAttributeToBuffer(size_t numOfElements, vector<GLfloat> &array, aiVector3D const &vector)
    {
      array.push_back(vector.x);
      array.push_back(vector.y);

      if(numOfElements > 2)
        array.push_back(vector.z);
    }

    void addBoneAttributeToBuffer(size_t numOfElements, vector<GLfloat> &array, GLfloat* vector)
    {
      array.push_back(vector[0]);
      if(numOfElements >= 2)
        array.push_back(vector[1]);
      if(numOfElements >= 4)
      {
        array.push_back(vector[2]);
        array.push_back(vector[3]);
      }
    }

    void addAttributeToBuffer(vector<GLfloat> &array, aiVector3D const &vector)
    {
      array.push_back(vector.x);
      array.push_back(vector.y);
      array.push_back(vector.z);
    }

    void fillArray(vector<GLfloat> &array, aiMesh const &mesh, bool normals, bool texCoords, bool binormals, bool tangents, bool bones, uint numOfTexCoords, uint numOfBoneWeights)
    {
      vector<GLfloat> boneIds;
      vector<GLfloat> boneWeights;


      if(bones)
      {
        vector<uint> vertexWeightIdx;
        vertexWeightIdx.resize(mesh.mNumVertices, 0);
        boneIds.resize(numOfBoneWeights * mesh.mNumVertices);
        boneWeights.resize(numOfBoneWeights * mesh.mNumVertices);

        for(size_t boneIdx = 0; boneIdx != mesh.mNumBones; ++boneIdx)
        {
          aiBone const &bone = *mesh.mBones[boneIdx];
          for(size_t weightIdx = 0; weightIdx != bone.mNumWeights; ++weightIdx)
          {
            aiVertexWeight const &weight = bone.mWeights[weightIdx];
            if(vertexWeightIdx[weight.mVertexId] >= numOfBoneWeights)
            {
              log(__FILE__, __LINE__, LogType::warning, "Model file contains more per vertex weights than needed");
              continue;
            }

            size_t idx = weight.mVertexId * numOfBoneWeights + vertexWeightIdx[weight.mVertexId];
            boneIds[idx] = boneIdx;
            boneWeights[idx] = weight.mWeight;

            ++vertexWeightIdx[weight.mVertexId];
          }
        }
      }

      // fill buffer
      for(size_t vert = 0; vert != mesh.mNumVertices; ++vert)
      {
        addAttributeToBuffer(array, mesh.mVertices[vert]);

        if(normals)
          addAttributeToBuffer(array, mesh.mNormals[vert]);

        if(texCoords)
          addTexCoordAttributeToBuffer(numOfTexCoords, array, mesh.mTextureCoords[0][vert]);

        if(binormals)
          addAttributeToBuffer(array, mesh.mBitangents[vert]);

        if(tangents)
          addAttributeToBuffer(array, mesh.mTangents[vert]);

        if(bones)
        {
          addBoneAttributeToBuffer(numOfBoneWeights, array, &boneIds[vert]);
          addBoneAttributeToBuffer(numOfBoneWeights, array, &boneWeights[vert]);
        }
      }
    }

    Mesh loadMesh(aiScene const &scene, std::vector<Scene::Option> const &options, size_t mesh, string const &filename)
    {
      vector<pair<internal::AttributeAccessor, Shader::Format>> attributes;
      attributes.push_back({Shader::vertex, Shader::vec3});

      if(in(options, Scene::texCoords3D) && scene.mMeshes[mesh]->mTextureCoords[0] == 0)
        throw log(filename, 0, LogType::error, "No texture coordinates present");



      uint numOfElements = 3;
      bool texCoords = false;
      bool normals = false;
      bool binormals = false;
      bool tangents = false;
      bool bones = false;
      size_t numOfTexCoords = 0;
      size_t numOfBoneWeights = 0;



      if(scene.mMeshes[mesh]->mNormals != 0 && ! in(options, Scene::noNormals))
      {
        numOfElements += 3;
        normals = true;

        attributes.push_back({Shader::normal, Shader::vec3});
      }

      if(scene.mMeshes[mesh]->mTextureCoords[0] != 0 && ! in(options, Scene::noTexCoords))
      {
        if(in(options, Scene::texCoords3D))
        {
          numOfElements += 3;
          numOfTexCoords = 3;
          attributes.push_back({Shader::texCoord, Shader::vec3});
        }
        else
        {
          numOfElements += 2;
          numOfTexCoords = 2;
          attributes.push_back({Shader::texCoord, Shader::vec2});
        }

        texCoords = true;
      }

      if(scene.mMeshes[mesh]->mBitangents != 0 && in(options, Scene::generateBinormals))
      {
        numOfElements += 3;
        binormals = true;

        attributes.push_back({Shader::binormal, Shader::vec3});
      }

      if(scene.mMeshes[mesh]->mTangents != 0 && in(options, Scene::generateTangents))
      {
        numOfElements += 3;
        tangents = true;

        attributes.push_back({Shader::tangent, Shader::vec3});
      }

      /*if(in(options, Scene::load8BoneWeights))
      {
        bones = true;
        numOfElements += 16;
        attributes.push_back({Shader::boneId, Shader::vec1});
        attributes.push_back({Shader::boneWeight, Shader::vec1});
      }
      else*/ if(in(options, Scene::load4BoneWeights))
      {
        bones = true;
        numOfBoneWeights = 4;
        numOfElements += 8;
        attributes.push_back({Shader::boneId, Shader::vec4});
        attributes.push_back({Shader::boneWeight, Shader::vec4});
      }
      else if(in(options, Scene::load2BoneWeights))
      {
        bones = true;
        numOfBoneWeights = 2;
        numOfElements += 4;
        attributes.push_back({Shader::boneId, Shader::vec2});
        attributes.push_back({Shader::boneWeight, Shader::vec2});
      }
      else if(in(options, Scene::load1BoneWeights))
      {
        bones = true;
        numOfBoneWeights = 1;
        numOfElements += 2;
        attributes.push_back({Shader::boneId, Shader::vec1});
        attributes.push_back({Shader::boneWeight, Shader::vec1});
      }

      if(bones && scene.mMeshes[mesh]->mNumBones == 0)
        throw log(filename, 0, LogType::error, "No bones present");

      // allocate buffer
      size_t numOfVertices = scene.mMeshes[mesh]->mNumVertices;

      vector<GLfloat> array;
      array.reserve(numOfVertices * numOfElements);

      fillArray(array, *scene.mMeshes[mesh], normals, texCoords, binormals, tangents, bones, numOfTexCoords, numOfBoneWeights);

      Mesh model(array.data(), numOfVertices, attributes);

      // Load indices
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


    aiScene const *loadScene(string const &filename, Assimp::Importer &importer, vector<Scene::Option> options = {})
    {
      // set flags
      uint flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                   aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure |
                   aiProcess_RemoveComponent;

      if(in(options, Scene::generateNormals) || in(options, Scene::regenerateNormals))
        flags |= aiProcess_GenSmoothNormals;

      if(in(options, Scene::generateTangents))
      {
        flags |= aiProcess_GenSmoothNormals;
        flags |= aiProcess_CalcTangentSpace;
      }
      if(in(options, Scene::generateBinormals))
      {
        flags |= aiProcess_GenSmoothNormals;
        flags |= aiProcess_CalcTangentSpace;
      }

      uint numOfWeights = 4;
      if(in(options, Scene::load4BoneWeights))
        numOfWeights = 4;
      else if(in(options, Scene::load2BoneWeights))
        numOfWeights = 2;
      else if(in(options, Scene::load1BoneWeights))
        numOfWeights = 1;

      int removeFlags = aiComponent_CAMERAS | aiComponent_COLORS | aiComponent_LIGHTS;
      if(in(options, Scene::generateNormals) || in(options, Scene::noNormals))
        removeFlags |= aiComponent_NORMALS;
      if(in(options, Scene::noTexCoords))
        removeFlags |= aiComponent_TEXCOORDS;

      importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, removeFlags);
      importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, numOfWeights);

      aiScene const *scene = importer.ReadFile(filename, flags);


      if(!scene)
        throw log(__FILE__, __LINE__, LogType::error, importer.GetErrorString());
      //if(scene->mFlags)

      if(in(options, Scene::texCoords3D) && scene->mMeshes[0]->mTextureCoords[0] == 0)
        throw log(filename, 0, LogType::error, "No texture coordinates present");

      return scene;
    }

    TextureManager stdManager(Filtering::trilinear);
  }

  namespace
  {
  vector<GLfloat> hiddenLoadPointData(std::string const &filename, aiScene const &scene, std::vector<Scene::Option> options)
  {
    // Find mesh properties
    bool texCoords = true;
    bool normals = true;
    bool binormals = true;
    bool tangents = true;
    bool bones = true;
    size_t numOfVertices = 0;

    for(size_t meshIdx = 0; meshIdx != scene.mNumMeshes; ++meshIdx)
    {
      aiMesh const &mesh = *scene.mMeshes[meshIdx];

      if(in(options, Scene::texCoords3D) && mesh.mTextureCoords[0] == 0)
        throw log(filename, 0, LogType::error, "No texture coordinates present");

      if(mesh.mNormals == 0 or in(options, Scene::noNormals))
        normals = false;

      if(mesh.mTextureCoords[0] == 0 or in(options, Scene::noTexCoords))
        texCoords = false;

      if(not in(options, Scene::generateBinormals))
        binormals = false;

      if(not in(options, Scene::generateTangents))
        tangents = false;

      if(not in(options, Scene::load4BoneWeights) and not in(options, Scene::load2BoneWeights) and not in(options, Scene::load1BoneWeights))
        bones = false;

      if(bones && mesh.mNumBones == 0)
        throw log(filename, 0, LogType::error, "No bones present");

      numOfVertices += mesh.mNumVertices;
    }

    // Find number of elements
    uint numOfElements = 3;
    uint numOfTexCoords = 2;
    uint numOfBoneWeights = 0;
    if(normals)
      numOfElements += 3;

    if(texCoords)
    {
      if(Scene::texCoords3D)
      {
        numOfElements += 3;
        numOfTexCoords = 3;
      }
      else
      {
        numOfElements += 2;
        numOfTexCoords = 2;
      }
    }

    if(binormals)
      numOfElements += 3;

    if(tangents)
      numOfElements += 3;

    /*if(bones and in(options, Scene::load8BoneWeights))
      numOfElements += 16;
    else*/ if(bones and in(options, Scene::load4BoneWeights))
    {
      numOfBoneWeights = 4;
      numOfElements += 8;
    }
    else if(bones and in(options, Scene::load2BoneWeights))
    {
      numOfBoneWeights = 2;
      numOfElements += 4;
    }
    else if(bones and in(options, Scene::load1BoneWeights))
    {
      numOfBoneWeights = 1;
      numOfElements += 2;
    }

    // load meshes
    vector<GLfloat> points;
    points.reserve(numOfElements * numOfVertices);

    for(size_t mesh = 0; mesh != scene.mNumMeshes; ++mesh)
      fillArray(points, *scene.mMeshes[mesh], normals, texCoords, binormals, tangents, bones, numOfTexCoords, numOfBoneWeights);

    return points;
  }
  }

  vector<GLfloat> Scene::loadPointData(string const &filename, vector<Option> options)
  {
    // load scene
    Assimp::Importer importer;
    aiScene const *scene = loadScene(filename, importer, options);

    return hiddenLoadPointData(filename, *scene, options);
  }



  bool extractBones(aiScene const &scene, aiNode const &node, Bone &bone)
  {
    // Check if this node is in the list of nodes
    bool isBone = false;
    for(size_t meshIdx = 0; meshIdx != scene.mNumMeshes; ++meshIdx)
    {
      aiMesh &mesh = *scene.mMeshes[meshIdx];
      for(size_t boneIdx = 0; boneIdx != mesh.mNumBones; ++boneIdx)
      {
        if(mesh.mBones[boneIdx]->mName == node.mName)
        {
          isBone = true;
          bone.setIndex(boneIdx);
          break;
        }
      }
      if(isBone)
        break;
    }

    if(not isBone)
    {
      for(size_t childIdx = 0; childIdx != node.mNumChildren; ++childIdx)
      {
        if(extractBones(scene, *node.mChildren[childIdx], bone))
          return true;
      }
      return false;
    }

    // isBone == true
    for(size_t childIdx = 0; childIdx != node.mNumChildren; ++childIdx)
    {
      Bone childBone;

      bool success = extractBones(scene, *node.mChildren[childIdx], childBone);
      if(success)
        bone.addChild(move(childBone));
    }

    return true;
  }

  pair<vector<GLfloat>, Bone> Scene::loadPointDataAndBones(string const &filename, vector<Option> options)
  {
    // load scene
    Assimp::Importer importer;
    aiScene const *scene = loadScene(filename, importer, options);

    Bone bone;
    bone.setIndex(-1);
    bool success = extractBones(*scene, *scene->mRootNode, bone);

    if(success == false)
      throw log(filename, 0, LogType::error, "Mesh does not contain any bones");

    return make_pair(hiddenLoadPointData(filename, *scene, options), move(bone));
  }

  Scene::Scene(std::string const &filename, std::vector<Option> list)
  :
      Scene(filename, stdManager, list)
  {
  }

  Scene::Scene(std::string const &filename, TextureManager &resources, std::vector<Option> options)
  {
    // load scene
    Assimp::Importer importer;
    aiScene const *scene = loadScene(filename, importer, options);

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
