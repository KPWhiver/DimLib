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

#include "DIM/dim.hpp"

#include <memory>

namespace dim
{
  template <typename Type>
	class Buffer
	{

		std::shared_ptr<GLuint> d_id;

	public:		
	
    enum Mode
		{
		  texture = GL_TEXTURE_BUFFER,
		  element = GL_ELEMENT_ARRAY_BUFFER,
		  data = GL_ARRAY_BUFFER,
		  uniform = GL_UNIFORM_BUFFER,
		  //pixelRead
		  //pixelWrite
		};
		Buffer(Buffer::Mode mode, size_t size, Type* data);
		
		void bind() const;
		void update(size_t bytes, Type* data);
		
		GLuint id() const;

	private:
		Mode d_bufferType;
	};

  template <typename Type>
  Buffer<Type>::Buffer(Buffer::Mode mode, size_t size, Type* data)
    :
      d_id(new GLuint(0), [](GLuint *ptr){glDeleteBuffers(1, ptr); delete ptr;}),
      d_bufferType(mode)
    {

      if(data == 0 && size == 0)
        return;

      glGenBuffers(1, d_id.get());
      glBindBuffer(d_bufferType, *d_id);
      glBufferData(d_bufferType, size * sizeof(Type), data, GL_STATIC_DRAW);
    }

    template <typename Type>
    GLuint Buffer<Type>::id() const
    {
      return *d_id;
    }

    template <typename Type>
    void Buffer<Type>::bind() const
    {
      if(*d_id == 0)
        return;

      glBindBuffer(d_bufferType, *d_id);
    }

    template <typename Type>
    void Buffer<Type>::update(size_t size, Type *data)
    {
      if(*d_id == 0)
        glGenBuffers(1, d_id.get());

      glBindBuffer(d_bufferType, *d_id);
      glBufferData(d_bufferType, size * sizeof(Type), 0, GL_DYNAMIC_DRAW);
      glBufferSubData(d_bufferType, 0, size * sizeof(Type), data);
    }

} /* namespace dim */
#endif /* DATABUFFER_HPP_ */
