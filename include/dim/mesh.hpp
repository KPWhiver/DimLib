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

#include "dim/buffer.hpp"

namespace dim
{

class Mesh
{   
  //GLuint d_interleavedVBO;
  //GLuint d_attribVBO[3];
  size_t d_attribSize[3];
  //GLuint d_indexVBO;
  Buffer<GLfloat> d_interleavedVBO;
  Buffer<GLfloat> d_attribVBO[3];
  Buffer<GLushort> d_indexVBO;
  size_t d_numVertices;
  size_t d_numTriangles;

  //GLuint d_instancingVBO;
  Buffer<GLfloat> d_instancingVBO;
  GLfloat* d_instancingArray;
  size_t d_maxLocations;
  
  static bool s_bindless;
  static bool s_instanced;
  static GLuint s_bound;
  static GLuint s_boundElem;

public:
  Mesh(std::string filename);
  
  Mesh(GLfloat* buffer, std::string const &format, size_t vertices);
  void add(GLfloat* buffer, std::string const &format, size_t vertices);
  
  void addElem(GLushort* buffer, size_t triangles);
  void addInst(GLfloat* buffer, size_t locations);

  Buffer<GLfloat> *buffer();
  Buffer<GLushort> &elementBuffer();
  Buffer<GLfloat> &instanceBuffer();

  //void update(GLfloat* buffer);
  //void updateElem(GLushort* buffer);
  //void updateInst(GLfloat* buffer, size_t locations);

  //void* get();

  void bind() const;
  void unbind() const;
  void bindElem() const;
  void unbindElem() const;
  void bindInst() const;

  void draw() const;
  void drawInst(size_t amount) const;

  static void initialize();

  GLuint id() const;

private:
  size_t readFormatString(std::string const &format);
};

}

#endif	/* MESHES_HPP */

