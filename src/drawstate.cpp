// drawable.cpp
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

#include <tuple>

#include "dim/drawstate.hpp"

using namespace std;

namespace dim
{
	DrawState::DrawState(Mesh const &mesh, Shader const &shader, std::vector<pair<Texture<GLubyte>, string>> const &textures)
	:
			d_mesh(mesh),
			d_shader(shader),
			d_textures(textures),
			d_culling(true)
	{
	}
	
	bool DrawState::culling() const
	{
	  return d_culling;
	}
	
	void DrawState::setCulling(bool culling)
	{
	  d_culling = culling;
	}

	Mesh const &DrawState::mesh() const
	{
		return d_mesh;
	}

	Shader const &DrawState::shader() const
	{
		return d_shader;
	}

	vector<pair<Texture<GLubyte>, string>> const &DrawState::textures() const
	{
		return d_textures;
	}

	bool DrawState::operator==(DrawState const &other) const
	{
	  return forward_as_tuple(d_shader.id(), d_textures[0].first.id(), d_mesh.id()) ==
	         forward_as_tuple(other.d_shader.id(), other.d_textures[0].first.id(), other.d_mesh.id());
	}

  bool DrawState::operator<(DrawState const &other) const
  {
    return forward_as_tuple(d_shader.id(), d_textures[0].first.id(), d_mesh.id()) <
           forward_as_tuple(other.d_shader.id(), other.d_textures[0].first.id(), other.d_mesh.id());


    /*if(d_shader.id() < other.d_shader.id())
      return true;
    if(d_shader.id() == other.d_shader.id())
    {
      if(d_mesh.id() < other.d_mesh.id())
        return true;
      if(d_mesh.id() == other.d_mesh.id())
      {
        if(d_textures[0].first.id() < other.d_textures[0].first.id())
          return true;
      }
    }
    return false;*/
  }
}
