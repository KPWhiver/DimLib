// drawstate.hpp
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

#ifndef DRAWSTATE_HPP
#define DRAWSTATE_HPP

#include <vector>


#include "DIM/mesh.hpp"
#include "DIM/shader.hpp"
#include "DIM/texture.hpp"

namespace dim
{

class DrawState
{
	Mesh d_mesh;
	Shader d_shader;
	std::vector<std::pair<Texture<GLubyte>, std::string>> d_textures;
	bool d_culling;

public:
	DrawState(Mesh const &mesh, Shader const &shader, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures);

    bool culling() const;
  
    void setCulling(bool culling);

	Mesh const &mesh() const;
	Shader const &shader() const;
	std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures() const;

	bool operator==(DrawState const &other) const;
  bool operator<(DrawState const &other) const;
};

}

#endif /* DRAWSTATE_HPP */
