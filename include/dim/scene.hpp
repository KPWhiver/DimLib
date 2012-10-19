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

#include "dim/mesh.hpp"
#include "dim/texture.hpp"

namespace dim
{

class DrawState
{
    friend class Scene;

    Mesh d_mesh;
    std::vector<std::pair<Texture<GLubyte>, std::string>> d_textures;
    bool d_culling;

    DrawState(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures);
  
  public:
    bool culling() const;
    std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures() const;
    Mesh const &mesh() const;
    
    void setCulling(bool culling);
    void setTextures(std::vector<std::pair<Texture<GLubyte>, std::string>> const &param);

    bool operator==(DrawState const &other) const;
    bool operator<(DrawState const &other) const;
};

class Scene
{


  std::vector<DrawState> d_states;

public:
  Scene() = default;
	Scene(std::string const &filename, Attribute const &vertex, Attribute const &normal, Attribute const &texCoord,
        Attribute const &binormal = {Attribute::unknown, Attribute::vec1}, Attribute const &tangent = {Attribute::unknown, Attribute::vec1});
	Scene(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures = {});
	void add(Mesh const &mesh, std::vector<std::pair<Texture<GLubyte>, std::string>> const &textures = {});

	void draw() const;

  DrawState &operator[](size_t idx);
  DrawState const &operator[](size_t idx) const;

  size_t size() const;

  bool operator==(Scene const &other) const;
  bool operator<(Scene const &other) const;
};

}

#endif /* DRAWSTATE_HPP */