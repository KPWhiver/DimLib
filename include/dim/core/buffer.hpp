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

#include "dim/core/dim.hpp"

#include <memory>
#include <iostream>

namespace dim
{
  template<typename Type>
  class ListAccessor
  {
      Type const *d_list;
      size_t d_size;

    public:
      ListAccessor(size_t a_size, Type const *list)
        :
          d_list(list),
          d_size(a_size)
      {
        if(d_size == 0)
          d_list = 0;
      }

      template<size_t Size>
      ListAccessor(std::array<Type, Size> const &list)
        :
          d_list(list.data()),
          d_size(Size)
      {
        if(d_size == 0)
          d_list = 0;
      }

      ListAccessor(std::vector<Type> const &list)
        :
          d_list(list.data()),
          d_size(list.size())
      {
        if(d_size == 0)
          d_list = 0;
      }

      ListAccessor(std::initializer_list<Type> const &list)
        :
          d_list(list.begin()),
          d_size(list.size())
      {
        if(d_size == 0)
          d_list = 0;
      }

      ListAccessor(ListAccessor const &other) = delete;

      size_t size() const
      {
        return d_size;
      }

      Type const *data() const
      {
        return d_list;
      }
  };

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

      Buffer(ListAccessor<Type> const &list);

      void bind(Mode mode) const;
      void update(ListAccessor<Type> const &list);

      Type* map(Access access);
      bool unmap();

      GLuint id() const;
      size_t size() const;

    private:
      Usage d_usage;
  };

  template<typename Type>
  Buffer<Type>::Buffer(ListAccessor<Type> const &list)
      :
          d_id(new GLuint(0), [](GLuint *ptr)
          { glDeleteBuffers(1, ptr);
            delete ptr;}),
          d_size(list.size()),
          d_usage(constant)
  {
    glGenBuffers(1, d_id.get());
    glBindBuffer(data, id());
    glBufferData(data, d_size * sizeof(Type), list.data(), d_usage);
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
  void Buffer<Type>::update(ListAccessor<Type> const &list)
  {
    d_size = list.size();
    glBindBuffer(data, id());
    glBufferData(data, d_size * sizeof(Type), 0, GL_DYNAMIC_DRAW);
    glBufferSubData(data, 0, d_size * sizeof(Type), list.data());
  }

  template<typename Type>
  Type* Buffer<Type>::map(Access access)
  {
    glBindBuffer(data, id());
    Type* ptr = reinterpret_cast<Type*>(glMapBuffer(data, access));
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
