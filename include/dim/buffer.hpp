// buffer.hpp
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

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include "dim/dim.hpp"

#include <memory>
#include <iostream>

namespace dim
{
  enum class BufferType
  {
    texture = GL_TEXTURE_BUFFER,
    element = GL_ELEMENT_ARRAY_BUFFER,
    data = GL_ARRAY_BUFFER,
    uniform = GL_UNIFORM_BUFFER,
    transformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
  //pixelRead
  //pixelWrite
  };

  template<typename Type>
  class Buffer
  {

      std::shared_ptr<GLuint> d_id;
      size_t d_size;

    public:

      enum Mode: GLuint
      {
        texture = GL_TEXTURE_BUFFER,
        element = GL_ELEMENT_ARRAY_BUFFER,
        data = GL_ARRAY_BUFFER,
        uniform = GL_UNIFORM_BUFFER,
        transformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
      //pixelRead
      //pixelWrite
      };

      enum Access: GLuint
      {
        read = GL_READ_ONLY,
        write = GL_WRITE_ONLY,
        readWrite = GL_READ_WRITE,
      };

      enum Usage: GLuint
      {
        constant = GL_STATIC_DRAW,
        dynamic = GL_DYNAMIC_DRAW,
        stream = GL_STREAM_DRAW,
      };

      Buffer(size_t size, Type* buffer);

      void bind(Mode mode) const;
      void update(size_t bytes, Type* buffer);

      Type* map(Access access);
      bool unmap();

      GLuint id() const;
      size_t size() const;

    private:
      Usage d_usage;
  };

  template<typename Type>
  Buffer<Type>::Buffer(size_t size, Type* buffer)
      :
          d_id(new GLuint, [](GLuint *ptr)
          { glDeleteBuffers(1, ptr);}),
          d_size(size),
          d_usage(constant)
  {
    glGenBuffers(1, d_id.get());
    glBindBuffer(data, id());
    glBufferData(data, size * sizeof(Type), buffer, d_usage);
  }

  template<typename Type>
  GLuint Buffer<Type>::id() const
  {
    return *d_id;
  }

  template<typename Type>
  size_t Buffer<Type>::size() const
  {
    return d_size;
  }

  template<typename Type>
  void Buffer<Type>::bind(Mode mode) const
  {
    glBindBuffer(mode, id());
  }

  template<typename Type>
  void Buffer<Type>::update(size_t size, Type *buffer)
  {
    d_size = size;
    glBindBuffer(data, id());
    glBufferData(data, size * sizeof(Type), 0, GL_DYNAMIC_DRAW);
    glBufferSubData(data, 0, size * sizeof(Type), buffer);
  }

  template<typename Type>
  Type* Buffer<Type>::map(Access access)
  {
    glBindBuffer(data, id());
    Type* ptr = reinterpret_cast<GLfloat*>(glMapBuffer(data, access));
    if(ptr == 0)
      log(__FILE__, __LINE__, LogType::warning, "OpenGL failed to map buffer");

    return ptr;
  }

  template<typename Type>
  bool Buffer<Type>::unmap()
  {
    glBindBuffer(data, id());
    return glUnmapBuffer(data);
  }

} /* namespace dim */
#endif /* DATABUFFER_HPP_ */
