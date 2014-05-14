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
  namespace internal
  {
    class AttributeAccessor
    {
        enum class UnionType
        {
          id,
          num,
          string
        };

        union
        {
            Shader::Attribute d_idAttribute;
            GLint d_numAttribute;
            std::string d_stringAttribute;
        };

        UnionType d_type;

      public:
        AttributeAccessor(Shader::Attribute attribute)
          :
            d_idAttribute(attribute),
            d_type(UnionType::id)
        {
        }

        AttributeAccessor(GLint attribute)
          :
            d_numAttribute(attribute),
            d_type(UnionType::num)
        {
        }

        AttributeAccessor(std::string const &attribute)
          :
            d_stringAttribute(attribute),
            d_type(UnionType::string)
        {
        }

        AttributeAccessor(AttributeAccessor const &other)
          :
            d_type(other.d_type)
        {
          if(d_type == UnionType::id)
            d_idAttribute = other.d_idAttribute;
          else if(d_type == UnionType::num)
            d_numAttribute = other.d_numAttribute;
          else //if(d_type == UnionType::string)
            d_stringAttribute = other.d_stringAttribute;
        }

        AttributeAccessor &operator=(AttributeAccessor const &other)
        {
          d_type = other.d_type;

          if(d_type == UnionType::id)
            d_idAttribute = other.d_idAttribute;
          else if(d_type == UnionType::num)
            d_numAttribute = other.d_numAttribute;
          else //if(d_type == UnionType::string)
            d_stringAttribute = other.d_stringAttribute;

          return *this;
        }

        bool operator==(AttributeAccessor const &other) const
        {
          if(d_type == UnionType::id)
            return d_idAttribute == other.d_idAttribute;
          else if(d_type == UnionType::num)
            return d_numAttribute == other.d_numAttribute;
          //else if(d_type == UnionType::string)
          return d_stringAttribute == other.d_stringAttribute;
        }

        template<typename Type>
        void set(Buffer<Type> const &buffer, Shader::Format format, uint floatStartOffset = 0, uint floatStride = 0) const
        {
          if(d_type == UnionType::id)
            Shader::set(d_idAttribute, buffer, format, floatStartOffset, floatStride);
          else if(d_type == UnionType::num)
            Shader::set(d_numAttribute, buffer, format, floatStartOffset, floatStride);
          else //if(d_type == UnionType::string)
            Shader::set(d_stringAttribute, buffer, format, floatStartOffset, floatStride);
        }

        void enable(Shader::Format format) const
        {
          if(d_type == UnionType::id)
            Shader::enableAttribute(d_idAttribute, format);
          else if(d_type == UnionType::num)
            Shader::enableAttribute(d_numAttribute, format);
          else //if(d_type == UnionType::string)
            Shader::enableAttribute(d_stringAttribute, format);
        }

        void disable(Shader::Format format) const
        {
          if(d_type == UnionType::id)
            Shader::disableAttribute(d_idAttribute, format);
          else if(d_type == UnionType::num)
            Shader::disableAttribute(d_numAttribute, format);
          else //if(d_type == UnionType::string)
            Shader::disableAttribute(d_stringAttribute, format);
        }

        void advancePerInstance(Shader::Format format, bool advance) const
        {
          if(d_type == UnionType::id)
            Shader::advanceAttributePerInstance(d_idAttribute, format, advance);
          else if(d_type == UnionType::num)
            Shader::advanceAttributePerInstance(d_numAttribute, format, advance);
          else //if(d_type == UnionType::string)
            Shader::advanceAttributePerInstance(d_stringAttribute, format, advance);
        }

        ~AttributeAccessor()
        {
          using std::string;

          if(d_type == UnionType::string)
            d_stringAttribute.~string();
        }
    };
  }

  class Bone
  {

      std::string d_name;
      glm::vec3 d_offset;
      std::vector<Bone> d_children;
      uint d_index;


    public:
      void setName(std::string const &name)
      {
        d_name = name;
      }

      void setOffset(glm::vec3 const &offset)
      {
        d_offset = offset;
      }

      glm::vec3 const &offset() const
      {
        return d_offset;
      }

      std::string const &name() const
      {
        return d_name;
      }

      std::vector<Bone> const &children() const
      {
        return d_children;
      }

      uint index() const
      {
        return d_index;
      }

      void setIndex(uint index)
      {
        d_index = index;
      }

      void addChild(Bone &&bone)
      {
        d_children.push_back(std::move(bone));
      }
  };

  class Mesh
  {
      std::vector<std::pair<internal::AttributeAccessor, Shader::Format>> d_formats;
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


      Mesh(GLfloat const *buffer, size_t numOfVertices, std::vector<std::pair<internal::AttributeAccessor, Shader::Format>> const &formats);
      Mesh(GLfloat const *buffer, size_t numOfVertices, internal::AttributeAccessor attribute, Shader::Format format);
      void addBuffer(GLfloat const *buffer, internal::AttributeAccessor attribute, Shader::Format format);

      void addElementBuffer(GLushort const *buffer, size_t numOfPolygons);
      void addInstanceBuffer(GLfloat const *buffer, size_t numOfLocations, Shader::Format format);

      Buffer<GLfloat> const &buffer();
      Buffer<GLushort> const &elementBuffer();
      Buffer<GLfloat> const &instanceBuffer();

      void updateBuffer(GLfloat const *buffer);
      void updateBuffer(GLfloat const *buffer, internal::AttributeAccessor attribute);
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
      int attributeIndex(internal::AttributeAccessor attribute) const;

      static void initialize();

      uint numOfElements() const;
  };
}

#endif	/* MESHES_HPP */

