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

#include "DIM/mesh.hpp"
#include "DIM/shader.hpp"

#include "Assimp/assimp.hpp"
#include "Assimp/aiScene.h"
#include "Assimp/aiPostProcess.h"

using namespace glm;
using namespace std;

namespace dim
{

	bool Mesh::s_bindless = false;
	bool Mesh::s_instanced = false;

	GLuint Mesh::s_bound = 0;
	GLuint Mesh::s_boundElem = 0;

	void Mesh::initialize()
	{
		s_bound = false;
		s_boundElem = false;
		s_bindless = false;
		if (GLEW_NV_shader_buffer_load && GLEW_NV_vertex_buffer_unified_memory)
		{
			/* It is safe to use bindless drawing. */
			s_bindless = true;
		}

		s_instanced = false;
		if (GLEW_ARB_draw_instanced && GLEW_ARB_instanced_arrays)
		{
			/* It is safe to use instancing. */
			s_instanced = true;
		}
	}

	Mesh::Mesh(string filename)
	:
	    d_attribSize{0},
      d_interleavedVBO(Buffer<GLfloat>::data, 0, 0),
      d_attribVBO({Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0), Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0), Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0)}),
      d_indexVBO(Buffer<GLushort>::element, 0, 0),
      d_numVertices(0),
      d_numTriangles(0),
      d_instancingVBO(Buffer<GLfloat>::element, 0, 0),
      d_instancingArray(0),
      d_maxLocations(0)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

		if (!scene)
			throw runtime_error(importer.GetErrorString());

		//size_t progress = 0;

		//for(size_t mesh = 0; mesh != scene->mNummeshes; ++mesh)
		//{
		size_t mesh = 0;
		string format("V3");
		size_t elements = 3;
		if (scene->mMeshes[mesh]->mNormals != 0)
		{
			format += "N3";
			elements += 3;
		}
		if (scene->mMeshes[mesh]->mTextureCoords[0] != 0)
		{
			format += "T2";
			elements += 2;
		}

		GLfloat *array = new GLfloat[scene->mMeshes[mesh]->mNumVertices * elements];
		d_numVertices = scene->mMeshes[mesh]->mNumVertices;

		//size_t numVert = scene->mMeshes[mesh]->mNumFaces * 3;
		size_t normals = 0;
		if (scene->mMeshes[mesh]->mNormals != 0)
			normals = 3;

		//for (size_t face = 0; face != scene->mMeshes[mesh]->mNumFaces; ++face)
		//{

			for (size_t idx = 0; idx != scene->mMeshes[mesh]->mNumVertices; ++idx)
			{
				//size_t idx = scene->mMeshes[0]->mFaces[face].mIndices[vert];
				size_t vertArrayIdx = idx * elements;

				array[vertArrayIdx] = scene->mMeshes[mesh]->mVertices[idx].x;
				array[vertArrayIdx + 1] = scene->mMeshes[mesh]->mVertices[idx].y;
				array[vertArrayIdx + 2] = scene->mMeshes[mesh]->mVertices[idx].z;

				if (scene->mMeshes[mesh]->mNormals != 0)
				{
					array[vertArrayIdx + 3] = scene->mMeshes[mesh]->mNormals[idx].x;
					array[vertArrayIdx + 4] = scene->mMeshes[mesh]->mNormals[idx].y;
					array[vertArrayIdx + 5] = scene->mMeshes[mesh]->mNormals[idx].z;
				}

				if (scene->mMeshes[mesh]->mTextureCoords[0] != 0)
				{
					array[vertArrayIdx + 3 + normals] = scene->mMeshes[mesh]->mTextureCoords[0][idx].x;
					array[vertArrayIdx + 3 + normals + 1] = scene->mMeshes[mesh]->mTextureCoords[0][idx].y;
				}
			}

		//}
		//progress += numVert;
		//}
		//float *array = reinterpret_cast<float*>(scene->mMeshes[0]->mVertices);
		add(array, format, scene->mMeshes[mesh]->mNumVertices);

		delete[] array;

		GLushort *indexArray = new GLushort[scene->mMeshes[mesh]->mNumFaces * 3];

	  for(size_t idx = 0; idx != scene->mMeshes[mesh]->mNumFaces; ++idx)
		{
		  indexArray[0 + idx * 3] = scene->mMeshes[mesh]->mFaces[idx].mIndices[0];
		  indexArray[0 + idx * 3 + 1] = scene->mMeshes[mesh]->mFaces[idx].mIndices[1];
		  indexArray[0 + idx * 3 + 2] = scene->mMeshes[mesh]->mFaces[idx].mIndices[2];
		}

		addElem(indexArray, scene->mMeshes[mesh]->mNumFaces);

		delete[] indexArray;
	}

	size_t Mesh::readFormatString(string const &format)
	{
		size_t mode = 0;

		if (format.length() > 6 || format.length() % 2 != 0)
			throw runtime_error("Error in Mesh format string");

		for (size_t idx = 0; idx != format.length(); idx += 2)
		{
			size_t slot;
			switch (format[idx])
			{
			case 'V':
				slot = 0;
				mode = 0;
				break;
			case 'N':
				slot = 1;
				mode = 1;
				break;
			case 'T':
				slot = 2;
				mode = 2;
				break;
			default:
				throw runtime_error("Error in Mesh format string");
			}
			if (not isdigit(format[idx + 1]))
				throw runtime_error("Error in Mesh format string");

			d_attribSize[slot] = format[idx + 1] - '0';
		}

		return mode;
	}

	Mesh::Mesh(GLfloat* buffer, string const &format, size_t vertices)
  :
      d_attribSize{0},
      d_interleavedVBO(Buffer<GLfloat>::data, 0, 0),
      d_attribVBO({Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0), Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0), Buffer<GLfloat>(Buffer<GLfloat>::data, 0, 0)}),
      d_indexVBO(Buffer<GLushort>::element, 0, 0),
      d_numVertices(vertices),
      d_numTriangles(0),
      d_instancingVBO(Buffer<GLfloat>::element, 0, 0),
      d_instancingArray(0),
      d_maxLocations(0)
	{
		add(buffer, format, vertices);
	}

	void Mesh::add(GLfloat* buffer, string const &format, size_t vertices)
	{
		size_t mode = readFormatString(format);
		size_t totalElements = d_attribSize[0] + d_attribSize[1] + d_attribSize[2];

		if (format.length() > 2)
		{
			//glGenBuffers(1, &d_interleavedVBO);
			//glBindBuffer(GL_ARRAY_BUFFER, d_interleavedVBO);
			//glBufferData(GL_ARRAY_BUFFER, vertices * totalElements * sizeof(GLfloat), buffer,
			//		GL_STATIC_DRAW);
		  d_interleavedVBO = Buffer<GLfloat>(Buffer<GLfloat>::data, vertices * totalElements, buffer);
		}
		else
		{
			//glGenBuffers(1, &d_attribVBO[mode]);
			//glBindBuffer(GL_ARRAY_BUFFER, d_attribVBO[mode]);
			//glBufferData(GL_ARRAY_BUFFER, vertices * d_attribSize[mode] * sizeof(GLfloat), buffer,
			//		GL_STATIC_DRAW);
      d_attribVBO[mode] = Buffer<GLfloat>(Buffer<GLfloat>::data, vertices * d_attribSize[mode], buffer);
		}
	}

	void Mesh::addElem(GLushort* buffer, size_t triangles)
	{
		d_numTriangles = triangles;

		if (d_indexVBO.id() != 0)
			return;

		//glGenBuffers(1, &d_indexVBO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_indexVBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, d_numTriangles * 3 * sizeof(GLushort), buffer,
		//		GL_STATIC_DRAW);
		d_indexVBO = Buffer<GLushort>(Buffer<GLushort>::element, d_numTriangles * 3, buffer);
	}

	void Mesh::addInst(GLfloat* buffer, size_t locations)
	{
		if (d_instancingVBO.id() != 0 || d_instancingArray != 0)
		  return;


		d_maxLocations = locations;
		if (s_instanced == true)
		{
			//glGenBuffers(1, &d_instancingVBO);
			//glBindBuffer(GL_ARRAY_BUFFER, d_instancingVBO);
			//glBufferData(GL_ARRAY_BUFFER, d_maxLocations * 3 * sizeof(GLfloat), buffer, GL_DYNAMIC_DRAW);
		  d_instancingVBO = Buffer<GLfloat>(Buffer<GLfloat>::data, d_maxLocations * 3, buffer);
		}
		else
		{
			d_instancingArray = buffer;
		}
	}

  Buffer<GLfloat> *Mesh::buffer()
  {
    if(d_interleavedVBO.id() == 0)
      return &d_attribVBO[0];

    return &d_interleavedVBO;
  }

  Buffer<GLushort> &Mesh::elementBuffer()
  {
    return d_indexVBO;
  }

  Buffer<GLfloat> &Mesh::instanceBuffer()
  {
    return d_instancingVBO;
  }

	void Mesh::bind() const
	{
		if(d_interleavedVBO.id() != 0)
		{
		  if(s_bound == d_interleavedVBO.id())
		    return;

			s_bound = d_interleavedVBO.id();
		}
		else /*d_attribVBO[0] != 0*/
		{
      if(s_bound == d_attribVBO[0].id())
        return;

			s_bound = d_attribVBO[0].id();
		}

		/* Enabling and disabling the correct arrays */
		if (d_attribSize[1] == 0)
			glDisableVertexAttribArray(1);

		if (d_attribSize[2] == 0)
			glDisableVertexAttribArray(2);

		size_t totalElements = d_attribSize[0] + d_attribSize[1] + d_attribSize[2];

		/* Set pointers when we're dealing with an interleaved */
		size_t offset = 0;
		if (d_interleavedVBO.id() != 0)
		{
			d_interleavedVBO.bind();

			for(size_t idx = 0; idx != 3; ++idx)
			{
			  if(d_attribSize[idx] != 0)
			  {
			    glVertexAttribPointer(idx, d_attribSize[idx], GL_FLOAT, GL_FALSE, totalElements * sizeof(GLfloat),
			                          reinterpret_cast<void*>(offset * sizeof(GLfloat)));
			    offset += d_attribSize[idx];
			  }
			}
		}
		else
		{
		  for(size_t idx = 0; idx != 3; ++idx)
		  {
		    if(d_attribSize[idx] != 0)
		    {
		      d_attribVBO[idx].bind();
		      //cout << d_attribVBO[idx].id() << ' ' << d_numVertices << '\n';
		      glVertexAttribPointer(idx, d_attribSize[idx], GL_FLOAT, GL_FALSE, 0, 0);
		    }
		  }
		}
		bindElem();
	}

	void Mesh::unbind() const
	{
		s_bound = 0;
		if (d_attribSize[1] == 0)
			glEnableVertexAttribArray(1);

		if (d_attribSize[2] == 0)
			glEnableVertexAttribArray(2);

		unbindElem();
	}

	void Mesh::bindElem() const
	{
	  if(d_indexVBO.id() == 0)
	    return;

		s_boundElem = d_indexVBO.id();

		d_indexVBO.bind();
	}

	void Mesh::unbindElem() const
	{
		s_boundElem = 0;
	}

	void Mesh::bindInst() const
	{

	}

	void Mesh::draw() const
	{

		if (s_bound == 0)
		{
			bind();
			s_bound = 0;
		}
		else if (s_bound != d_interleavedVBO.id() && s_bound != d_attribVBO[0].id())
		{
			bind();
			s_bound = 0;
		}

		if (d_indexVBO.id() != 0)
		{
			if (s_boundElem == 0)
			{
				bindElem();
				s_boundElem = 0;
			}
			glDrawElements(GL_TRIANGLES, d_numTriangles * 3, GL_UNSIGNED_SHORT, NULL);
		}
		else
		{
		  //cout << d_attribVBO[0].id() << ' ' << d_numVertices << '\n';
			glDrawArrays(GL_TRIANGLES, 0, d_numVertices);
		}

		if (s_bound == 0)
		{
			if (d_attribSize[1] == 0)
				glEnableVertexAttribArray(1);

			if (d_attribSize[2] == 0)
				glEnableVertexAttribArray(2);
		}

	}

	void Mesh::drawInst(size_t amount) const
	{

		if (s_bound == 0)
		{
			bind();
			s_bound = 0;
		}
		else if (s_bound != d_interleavedVBO.id() && s_bound != d_attribVBO[0].id())
		{
			bind();
			s_bound = 0;
		}

		if (s_instanced == true)
		{
			Shader::active().send(vec3(0.0f, -10000.0f, 0.0f), "in_placement");

			glEnableVertexAttribArray(3);

			d_instancingVBO.bind();
			glVertexAttribPointer((GLuint) 3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glVertexAttribDivisorARB((GLuint) 3, 1);
			if (d_indexVBO.id() != 0)
			{
				if (s_boundElem == 0)
				{
					bindElem();
					s_boundElem = 0;
				}
				glDrawElementsInstancedARB(GL_TRIANGLES, d_numTriangles * 3, GL_UNSIGNED_SHORT, NULL,
						amount);
			}
			else
			{
				glDrawArraysInstancedARB(GL_TRIANGLES, 0, d_numVertices, amount);
			}

			glDisableVertexAttribArray(3);
		}
		else
		{
			for (size_t idx = 0; idx < amount; idx += 3)
			{
				Shader::active().send(
						vec3(d_instancingArray[idx * 3], d_instancingArray[idx * 3 + 1],
								d_instancingArray[idx * 3 + 2]), "in_placement");
				if (d_indexVBO.id() != 0)
				{
					if (s_boundElem == 0)
					{
						bindElem();
						s_boundElem = 0;
					}
					glDrawElements(GL_TRIANGLES, d_numTriangles * 3, GL_UNSIGNED_SHORT, NULL);
				}
				else
				{
					glDrawArrays(GL_TRIANGLES, 0, d_numVertices);
				}
			}
		}

		if (s_bound == 0)
		{
			if (d_attribSize[1] == false)
				glEnableVertexAttribArray(1);

			if (d_attribSize[2] == false)
				glEnableVertexAttribArray(2);
		}
	}

	GLuint Mesh::id() const
	{
		if(d_interleavedVBO.id() != 0)
		  return d_interleavedVBO.id();

		if(d_attribVBO[0].id() != 0)
		  return d_attribVBO[0].id();

		return 0;
	}


}
