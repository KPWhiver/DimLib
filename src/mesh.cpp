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

#include "dim/mesh.hpp"
#include "dim/shader.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace glm;
using namespace std;

namespace dim
{
  // --- Attribute ---

  Attribute::Attribute(GLuint id, Format size)
      :
          d_id(id),
          d_format(size)
  {
  }

  Attribute::Attribute(std::string const &name, Format size)
      :
          d_name(name),
          d_id(unknown),
          d_format(size)
  {
  }

  Attribute::Attribute(Name id, Format size)
      :
          d_id(id),
          d_format(size)
  {
  }

  GLint Attribute::id() const
  {
    return d_id;
  }

  GLint Attribute::location() const
  {
    if(d_name != "")
      return 0; //TODO implement
    else if(d_id == unknown)
      log(__FILE__, __LINE__, LogType::error, "Can't return unknown id");

    return d_id;
  }

  Attribute::Format Attribute::format() const
  {
    return d_format;
  }

  uint Attribute::size() const
  {
    uint width = d_format % 10;
    uint height = d_format / 10;

    return width * height;
  }

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

  Mesh::Mesh(GLfloat* buffer, vector<Attribute> attributes, size_t numOfVertices, Shape shape, size_t idx)
      :
          d_interleavedVBO(0, 0),
          d_indexVBO(0, 0),
          d_numOfVertices(numOfVertices),
          d_numOfTriangles(0),
          d_instancingVBO(0, 0),
          d_maxLocations(0),
          d_shape(shape),
          d_attributes(attributes),
          d_instanceAttribute(Attribute::instance, Attribute::vec1)
  {
    // static initialize
    if(s_initialized == false)
      initialize();

    addBuffer(buffer, idx);
  }

  void Mesh::addBuffer(GLfloat* buffer, size_t idx)
  {
    uint varNumOfElements = numOfElements();

    if(idx == interleaved || (idx == 0 && d_attributes.size() == 1))
    {
      if(d_interleavedVBO.size() == 0)
      {
        d_interleavedVBO = Buffer<GLfloat>(d_numOfVertices * varNumOfElements, buffer);
      }
      else
      {
        log(__FILE__, __LINE__, LogType::error, "Interleaved arrays can only be added at the first call");
        return;
      }
    }
    else
    {
      // Make a new buffer
      if(d_interleavedVBO.size() == 0)
      {
        GLfloat *array = new GLfloat[d_numOfVertices * varNumOfElements];

        updateBuffer(array, buffer, varNumOfElements, idx);

        d_interleavedVBO = Buffer<GLfloat>(d_numOfVertices * varNumOfElements, array);
        delete[] array;
      }
      // Paste into the old buffer
      else
      {
        updateBuffer(buffer, idx);
      }
    }
  }

  void Mesh::updateBuffer(GLfloat* buffer, size_t idx)
  {
    uint varNumOfElements = numOfElements();

    if(d_interleavedVBO.size() == 0)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't update a Mesh if no buffers have been added yet");
      return;
    }

    if(idx == interleaved)
      d_interleavedVBO.update(d_numOfVertices * varNumOfElements, buffer);
    else
    {
      GLfloat* ptr = d_interleavedVBO.map(Buffer<GLfloat>::write);

      if(ptr == 0)
        return;

      updateBuffer(ptr, buffer, varNumOfElements, idx);

      d_interleavedVBO.unmap();
    }
  }

  void Mesh::updateBuffer(GLfloat *target, GLfloat *source, uint varNumOfElements, size_t idx)
  {
    for(size_t vert = 0; vert != d_numOfVertices; ++vert)
    {
      uint arrayIdxOffset = 0;
      size_t arrayIdxStart = vert * varNumOfElements;

      for(size_t attrib = 0; attrib != d_attributes.size(); ++attrib)
      {
        uint size = d_attributes[attrib].size();

        if(idx == attrib)
        {
          for(size_t coor = 0; coor != size; ++coor)
          {
            target[arrayIdxStart + arrayIdxOffset + coor] = source[vert * size + coor];
          }
        }
        arrayIdxOffset += size;
      }
    }
  }

  void Mesh::addElementBuffer(GLushort* buffer, size_t numOfTriangles)
  {
    d_numOfTriangles = numOfTriangles;

    if(d_indexVBO.size() != 0)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't add an element buffer, it's has already been added");
      return;
    }

    d_indexVBO = Buffer<GLushort>(d_numOfTriangles * 3, buffer);
  }

  void Mesh::addInstanceBuffer(GLfloat* buffer, size_t locations, Attribute const &attrib)
  {
    if(d_instancingVBO.size() != 0)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't add an instance buffer, it's has already been added");
      return;
    }

    d_instanceAttribute = attrib;

    d_maxLocations = locations;
    d_instancingVBO = Buffer<GLfloat>(d_maxLocations * 3, buffer);
  }

  void Mesh::updateElementBuffer(GLushort* buffer)
  {
    if(d_indexVBO.size() == 0)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't update a element buffer if no element buffers have been added yet");
      return;
    }

    d_indexVBO.update(d_numOfTriangles * 3, buffer);
  }

  void Mesh::updateInstanceBuffer(GLfloat* buffer, size_t locations)
  {
    if(d_instancingVBO.size() == 0)
    {
      log(__FILE__, __LINE__, LogType::error, "Can't update a instance buffer if no instance buffers have been added yet");
      return;
    }

    d_maxLocations = locations;
    d_instancingVBO.update(d_maxLocations * 3, buffer);

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
    for(Attribute attrib : d_attributes)
      varNumOfElements += attrib.size();

    return varNumOfElements;
  }

  void Mesh::bind() const
  {
    if(s_bound == d_interleavedVBO.id())
      return;

    s_bound = d_interleavedVBO.id();

    for(Attribute attrib : d_attributes)
      glEnableVertexAttribArray(attrib.id());

    uint varNumOfElements = numOfElements();

    // set pointers when we're dealing with an interleaved
    size_t offset = 0;
    d_interleavedVBO.bind(Buffer<GLfloat>::data);

    for(Attribute attrib : d_attributes)
    {
      glVertexAttribPointer(attrib.id(), attrib.size(), GL_FLOAT, GL_FALSE, varNumOfElements * sizeof(GLfloat),
                            reinterpret_cast<void*>(offset * sizeof(GLfloat)));
      offset += attrib.size();
    }

    //bindElement();
  }

  void Mesh::unbind() const
  {
    s_bound = 0;
    for(Attribute attrib : d_attributes)
      glDisableVertexAttribArray(attrib.id());

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

  void Mesh::draw() const
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
      glDrawElements(GL_TRIANGLES, d_numOfTriangles * 3, GL_UNSIGNED_SHORT, 0);
    }
    else
    {
      glDrawArrays(GL_TRIANGLES, 0, d_numOfVertices);
    }

    if(s_bound == 0)
    {
      unbind();
    }

  }

  void Mesh::drawInstanced(size_t amount) const
  {

    if(s_bound == 0 || s_bound != d_interleavedVBO.id())
    {
      bind();
      s_bound = 0;
    }

    glEnableVertexAttribArray(d_instanceAttribute.id());

    d_instancingVBO.bind(Buffer<GLfloat>::data);
    glVertexAttribPointer(d_instanceAttribute.id(), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(d_instanceAttribute.id(), 1);
    if(d_indexVBO.size() != 0)
    {
      if(s_boundElem == 0)
      {
        bindElement();
        s_boundElem = 0;
      }
      glDrawElementsInstanced(GL_TRIANGLES, d_numOfTriangles * 3, GL_UNSIGNED_SHORT, NULL, amount);
    }
    else
    {
      glDrawArraysInstanced(GL_TRIANGLES, 0, d_numOfVertices, amount);
    }

    glDisableVertexAttribArray(d_instanceAttribute.id());

    if(s_bound == 0)
    {
      unbind();
    }
  }

  GLuint Mesh::id() const
  {
    return d_interleavedVBO.id();
  }

  bool Mesh::culling() const
  {
    return d_culling;
  }

  void Mesh::setCulling(bool culling)
  {
    d_culling = culling;
  }

  vector<pair<Texture<GLubyte>, string>> const &Mesh::textures() const
  {
    return d_textures;
  }

  void setTextures(std::vector<std::pair<Texture<GLubyte>, std::string>> const &param)
  {
    d_textures = param;
  }

}
