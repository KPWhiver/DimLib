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

#include "dim/core/buffer.hpp"
#include "dim/core/texture.hpp"

namespace dim
{
  class Attribute
  {
      std::string d_name;
      GLint d_id;

    public:
      enum Name: GLint
      {
        vertex = 0,
        normal = 1,
        texCoord = 2,
        binormal = 3,
        tangent = 4,
        instance = 5,
        unknown = -1,
      };

      enum Format: uint
      {
        vec1 = 11,
        vec2 = 12,
        vec3 = 13,
        vec4 = 14,
        mat2 = 22,
        mat2x3 = 23,
        mat2x4 = 24,
        mat3x2 = 32,
        mat3 = 33,
        mat3x4 = 34,
        mat4x2 = 42,
        mat4x3 = 43,
        mat4 = 44,
      };

      Attribute(GLuint id, Format size);
      Attribute(std::string const &name, Format size);
      Attribute(Name name, Format size);

      GLint id() const;
      GLint location() const;
      Format format() const;
      uint size() const;

    private:
      Format d_format;

  };

  class Mesh
  {
      Buffer<GLfloat> d_interleavedVBO;
      Buffer<GLushort> d_indexVBO;
      size_t d_numOfVertices;
      size_t d_numOfTriangles;

      Buffer<GLfloat> d_instancingVBO;
      size_t d_maxLocations;

      static bool s_bindless;
      static bool s_instanced;
      static GLuint s_bound;
      static GLuint s_boundElem;

      static bool s_initialized;

    public:
      enum Shape: uint
      {
        triangle,
        line,
        point,
      };
      static size_t const interleaved = std::numeric_limits<size_t>::max();

      Mesh(GLfloat* buffer, std::vector<Attribute> attributes, size_t numOfVertices, Shape shape, size_t idx);
      void addBuffer(GLfloat* buffer, size_t idx);

      void addElementBuffer(GLushort* buffer, size_t numOfPolygons);
      void addInstanceBuffer(GLfloat* buffer, size_t numOfLocations, Attribute const &attrib);

      Buffer<GLfloat> const &buffer();
      Buffer<GLushort> const &elementBuffer();
      Buffer<GLfloat> const &instanceBuffer();

      void updateBuffer(GLfloat* buffer, size_t idx);
      void updateElementBuffer(GLushort* buffer);
      void updateInstanceBuffer(GLfloat* buffer, size_t locations);

      void bind() const;
      void unbind() const;
      void bindElement() const;
      void unbindElement() const;
      //void bindInstance() const;

      void draw() const;
      void drawInstanced(size_t numOfPolygons) const;

      GLuint id() const;

    private:
      Shape d_shape;
      std::vector<Attribute> d_attributes;
      Attribute d_instanceAttribute;

      static void initialize();
      void updateBuffer(GLfloat *target, GLfloat *source, GLuint varNumOfElements, size_t idx);

      uint numOfElements() const;
  };
}

#endif	/* MESHES_HPP */

