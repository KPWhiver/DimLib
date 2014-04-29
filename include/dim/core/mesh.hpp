// mesh.hpp
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

#ifndef MESHES_HPP
#define	MESHES_HPP

//#include <vector>
#include <string>
#include <limits>
#include <vector>

#include "dim/core/shader.hpp"
#include "dim/core/buffer.hpp"
#include "dim/core/texture.hpp"

namespace dim
{
  class Mesh
  {
      std::vector<std::pair<Shader::Attribute, Shader::Format>> d_formats;
      Shader::Format d_instanceFormat;

      Buffer<GLfloat> d_interleavedVBO;
      Buffer<GLushort> d_indexVBO;
      size_t d_numOfVertices;
      size_t d_numOfTriangles;

      Buffer<GLfloat> d_instancingVBO;
      size_t d_maxLocations;

      std::vector<Buffer<GLfloat>> d_additionalVBOs;

      static bool s_bindless;
      static bool s_instanced;
      static GLuint s_bound;
      static GLuint s_boundElem;

      static bool s_initialized;
    public:
      enum Shape: GLenum
      {
        triangle = GL_TRIANGLES,
        line = GL_LINES,
        point = GL_POINTS
      };


      Mesh(GLfloat const *buffer, size_t numOfVertices, std::vector<std::pair<Shader::Attribute, Shader::Format>> const &formats);
      Mesh(GLfloat const *buffer, size_t numOfVertices, Shader::Attribute attribute, Shader::Format format);
      void addBuffer(GLfloat const *buffer, Shader::Attribute attribute, Shader::Format format);

      void addElementBuffer(GLushort const *buffer, size_t numOfPolygons);
      void addInstanceBuffer(GLfloat const *buffer, size_t numOfLocations, Shader::Format format);

      Buffer<GLfloat> const &buffer();
      Buffer<GLushort> const &elementBuffer();
      Buffer<GLfloat> const &instanceBuffer();

      void updateBuffer(GLfloat const *buffer);
      void updateBuffer(GLfloat const *buffer, Shader::Attribute attribute);
      void updateElementBuffer(GLushort const *buffer);
      void updateInstanceBuffer(GLfloat const *buffer, size_t locations);

      void bind() const;
      void unbind() const;
      void bindElement() const;
      void unbindElement() const;
      //void bindInstance() const;

      void draw(Shape shape = triangle) const;
      void drawInstanced(size_t numOfPolygons, Shape shape = triangle) const;

      GLuint id() const;

    private:
      int attributeIndex(Shader::Attribute attribute) const;

      static void initialize();

      uint numOfElements() const;
  };
}

#endif	/* MESHES_HPP */

