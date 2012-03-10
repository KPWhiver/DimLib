// shaderbuffer.cpp
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

#include "DIM/shaderbuffer.hpp"

namespace dim
{
  ShaderBuffer::ShaderBuffer()
  :
  	d_programId(glCreateProgram()),
    d_vertexShaderId(glCreateShader(GL_VERTEX_SHADER)),
    d_fragmentShaderId(glCreateShader(GL_FRAGMENT_SHADER))

  {
  }

  ShaderBuffer::~ShaderBuffer()
  {
    glDetachShader(d_programId, d_vertexShaderId);
    glDetachShader(d_programId, d_fragmentShaderId);

    glDeleteShader(d_vertexShaderId);
    glDeleteShader(d_fragmentShaderId);
    glDeleteProgram(d_programId);
  }
  
  GLuint ShaderBuffer::program() const
  {
    return d_programId;
  }
  
  GLuint ShaderBuffer::vertex() const
  {
    return d_vertexShaderId;
  }
  
  GLuint ShaderBuffer::fragment() const
  {
    return d_fragmentShaderId;
  }
}
