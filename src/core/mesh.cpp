//      mesh.cpp
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

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "dim/core/mesh.hpp"
#include "dim/core/shader.hpp"

using namespace glm;
using namespace std;

namespace dim
{
  // --- Mesh ---

  bool Mesh::s_bindless = false;
  bool Mesh::s_instanced = false;

  GLuint Mesh::s_bound = 0;
  GLuint Mesh::s_boundElem = 0;

  bool Mesh::s_initialized = false;

  void Mesh::initialize()
  {
    s_initialized = true;

    s_bound = false;
    s_boundElem = false;
    s_bindless = false;
    if(GLEW_NV_shader_buffer_load && GLEW_NV_vertex_buffer_unified_memory )
    {
      /* It is safe to use bindless drawing. */
      s_bindless = true;
    }

    s_instanced = false;
    if(GLEW_ARB_draw_instanced && GLEW_ARB_instanced_arrays )
    {
      /* It is safe to use instancing. */
      s_instanced = true;
    }
  }

  Mesh::Mesh(GLfloat const *buffer, size_t numOfVertices, internal::AttributeAccessor attribute, Shader::Format format)
    :
      d_formats{{attribute, format}},
      d_instanceFormat(Shader::vec1),
      d_interleavedVBO(numOfVertices * numOfElements(), buffer),
      d_indexVBO(0, 0),
      d_numOfVertices(numOfVertices),
      d_numOfTriangles(0),
      d_instancingVBO(0, 0),
      d_maxLocations(0)
  {
    // static initialize
    if(s_initialized == false)
      initialize();
  }

  Mesh::Mesh(GLfloat const *buffer, size_t numOfVertices, std::vector<std::pair<internal::AttributeAccessor, Shader::Format>> const &formats)
    :
      d_formats(formats),
      d_instanceFormat(Shader::vec1),
      d_interleavedVBO(numOfVertices * numOfElements(), buffer),
      d_indexVBO(0, 0),
      d_numOfVertices(numOfVertices),
      d_numOfTriangles(0),
      d_instancingVBO(0, 0),
      d_maxLocations(0)

  {
    // static initialize
    if(s_initialized == false)
      initialize();
  }

  void Mesh::addBuffer(GLfloat const *buffer, internal::AttributeAccessor attribute, Shader::Format format)
  {
    if(attributeIndex(attribute) != -1)
      throw log(__FILE__, __LINE__, LogType::error, "Trying to add a buffer to a mesh that is already added");

    d_additionalVBOs.emplace_back(d_numOfVertices * internal::formatSize(format), buffer);
    d_formats.push_back({attribute, format});
  }

  void Mesh::updateBuffer(GLfloat const *buffer)
  {
    if(d_interleavedVBO.size() == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Unable to call Mesh::updateBuffer(buffer) on an interleaved mesh");

    d_interleavedVBO.update(d_numOfVertices * numOfElements(), buffer);
  }

  void Mesh::updateBuffer(GLfloat const *buffer, internal::AttributeAccessor attribute)
  {
    int index = attributeIndex(attribute);

    if(index == -1 || (index != 0 && d_additionalVBOs[index - 1].size() == 0) || (index == 0 && d_interleavedVBO.size() == 0))
      throw log(__FILE__, __LINE__, LogType::error, "Unable to call Mesh::updateBuffer(buffer, attribute) on an attribute that has not been added first");

    uint formatS = internal::formatSize(d_formats[index].second);

    if(index == 0)
      d_interleavedVBO.update(d_numOfVertices * formatS, buffer);
    else
      d_additionalVBOs[index - 1].update(d_numOfVertices * formatS, buffer);
  }

  void Mesh::addElementBuffer(GLushort const *buffer, size_t numOfTriangles)
  {
    d_numOfTriangles = numOfTriangles;

    if(d_indexVBO.size() != 0)
      throw log(__FILE__, __LINE__, LogType::error, "Can't add an element buffer, it's has already been added");

    d_indexVBO = Buffer<GLushort>(d_numOfTriangles * 3, buffer);
  }

  void Mesh::addInstanceBuffer(GLfloat const *buffer, size_t locations, Shader::Format format)
  {
    if(d_instancingVBO.size() != 0)
      throw log(__FILE__, __LINE__, LogType::error, "Can't add an instance buffer, it's has already been added");

    d_instanceFormat = format;

    d_maxLocations = locations;
    d_instancingVBO = Buffer<GLfloat>(d_maxLocations * internal::formatSize(d_instanceFormat), buffer);
  }

  void Mesh::updateElementBuffer(GLushort const *buffer)
  {
    if(d_indexVBO.size() == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Can't update a element buffer if no element buffers have been added yet");

    d_indexVBO.update(d_numOfTriangles * 3, buffer);
  }

  void Mesh::updateInstanceBuffer(GLfloat const *buffer, size_t locations)
  {
    if(d_instancingVBO.size() == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Can't update a instance buffer if no instance buffers have been added yet");

    d_maxLocations = locations;
    d_instancingVBO.update(d_maxLocations * internal::formatSize(d_instanceFormat), buffer);
  }

  Buffer<GLfloat> const &Mesh::buffer()
  {
    return d_interleavedVBO;
  }

  Buffer<GLushort> const &Mesh::elementBuffer()
  {
    return d_indexVBO;
  }

  Buffer<GLfloat> const &Mesh::instanceBuffer()
  {
    return d_instancingVBO;
  }

  uint Mesh::numOfElements() const
  {
    uint varNumOfElements = 0;
    for(auto format : d_formats)
      varNumOfElements += internal::formatSize(format.second);

    return varNumOfElements;
  }

  int Mesh::attributeIndex(internal::AttributeAccessor attribute) const
  {
    for(size_t idx = 0; idx != d_formats.size(); ++idx)
    {
      if(d_formats[idx].first == attribute)
        return idx;
    }
    return -1;
  }

  void Mesh::bind() const
  {
    if(s_bound == d_interleavedVBO.id())
      return;

    s_bound = d_interleavedVBO.id();

    for(auto format : d_formats)
      format.first.enable(format.second);

    // set pointers when we're dealing with an interleaved
    if(d_additionalVBOs.size() == 0)
    {
      uint varNumOfElements = numOfElements();
      size_t offset = 0;
      d_interleavedVBO.bind(Buffer<GLfloat>::data);

      for(auto format : d_formats)
      {
        format.first.set(d_interleavedVBO, format.second, offset, varNumOfElements);
        offset += internal::formatSize(format.second);
      }
    }
    // Set pointers when we're dealing with individual buffer
    else
    {
      d_interleavedVBO.bind(Buffer<GLfloat>::data);
      d_formats[0].first.set(d_interleavedVBO, d_formats[0].second);

      for(uint idx = 0; idx != d_additionalVBOs.size(); ++idx)
      {
        d_additionalVBOs[idx].bind(Buffer<GLfloat>::data);
        d_formats[idx + 1].first.set(d_additionalVBOs[idx], d_formats[idx + 1].second);
      }
    }

    //bindElement();
  }

  void Mesh::unbind() const
  {
    s_bound = 0;
    for(auto format : d_formats)
      format.first.disable(format.second);

    //unbindElement();
  }

  void Mesh::bindElement() const
  {
    if(d_indexVBO.size() == 0)
      return;

    s_boundElem = d_indexVBO.id();

    d_indexVBO.bind(Buffer<GLushort>::element);
  }

  void Mesh::unbindElement() const
  {
    s_boundElem = 0;
  }

  void Mesh::draw(Shape shape) const
  {

    if(s_bound == 0 || s_bound != d_interleavedVBO.id())
    {
      bind();
      s_bound = 0;
    }

    if(d_indexVBO.size() != 0)
    {
      if(s_boundElem == 0)
      {

        bindElement();
        s_boundElem = 0;
      }
      glDrawElements(shape, d_numOfTriangles * 3, GL_UNSIGNED_SHORT, 0);
    }
    else
    {
      glDrawArrays(shape, 0, d_numOfVertices);
    }

    if(s_bound == 0)
    {
      unbind();
    }

  }

  void Mesh::drawInstanced(size_t numOfPolygons, Shape shape) const
  {

    if(s_bound == 0 || s_bound != d_interleavedVBO.id())
    {
      bind();
      s_bound = 0;
    }

    Shader::enableAttribute(Shader::instance, d_instanceFormat);

    d_instancingVBO.bind(Buffer<GLfloat>::data);
    Shader::set(Shader::instance, d_instancingVBO, d_instanceFormat);
    Shader::advanceAttributePerInstance(Shader::instance, d_instanceFormat, true);

    if(d_indexVBO.size() != 0)
    {
      if(s_boundElem == 0)
      {
        bindElement();
        s_boundElem = 0;
      }
      glDrawElementsInstanced(shape, d_numOfTriangles * 3, GL_UNSIGNED_SHORT, NULL, numOfPolygons);
    }
    else
    {
      glDrawArraysInstanced(shape, 0, d_numOfVertices, numOfPolygons);
    }

    Shader::disableAttribute(Shader::instance, d_instanceFormat);

    if(s_bound == 0)
    {
      unbind();
    }
  }

  GLuint Mesh::id() const
  {
    return d_interleavedVBO.id();
  }
}
