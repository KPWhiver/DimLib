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

#include "DIM/drawable.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

namespace dim
{
  mat4 Drawable::matrix() const
  {
    mat4 modelMat;
    modelMat = translate(modelMat, d_coor);
    if(d_rot.x != 0)
      modelMat = translate(modelMat, vec3(d_rot.x, 0, 0));
    if(d_rot.y != 0)
      modelMat = translate(modelMat, vec3(0, 0, d_rot.z));
    if(d_rot.z != 0)
      modelMat = translate(modelMat, vec3(0, d_rot.y, 0));

    return modelMat;
  }

  Drawable::Drawable(vec3 const &coor, vec3 const &rot, float radius)
      : d_coor(coor), d_rot(rot), d_radius(radius)
  {
  }

  Drawable::~Drawable()
  {
  }

  Drawable::Id Drawable::id() const
  {
    return d_id;
  }

  glm::vec3 Drawable::coor() const
  {
    return d_coor;
  }

  void Drawable::setCoor(const vec3 & coor)
  {
    d_coor = coor;
  }

  glm::vec3 const &Drawable::rot() const
  {
    return d_rot;
  }

  void Drawable::draw(const DrawState & state)
  {
    v_draw(state);
  }

  void Drawable::v_draw(const DrawState & state)
  {
    state.mesh().draw();
  }

  std::vector<DrawState> const &Drawable::drawStates() const
  {
    return v_drawStates();
  }

  void Drawable::v_insert(ostream &out) const
  {
	  out << d_coor.x << ' ' << d_coor.y << ' ' << d_coor.z << ' ' << d_rot.x << ' ' << d_rot.y << ' ' << d_rot.z << '\n';
    //out.write(reinterpret_cast<const char*>(&d_coor.x), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.y), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.z), 4);
    //out.write(reinterpret_cast<const char*>(&d_xRot), 4);
    //out.write(reinterpret_cast<const char*>(&d_yRot), 4);
  }

  void Drawable::v_extract(istream &in)
  {
	  in >> d_coor.x >> d_coor.y >> d_coor.z >> d_rot.x >> d_rot.y >> d_rot.z;

	  d_coor/=100.0f;
	  d_rot/=100;

    //in.read(reinterpret_cast<char*>(&d_coor.x), 4);
    //in.read(reinterpret_cast<char*>(&d_coor.y), 4);
    //in.read(reinterpret_cast<char*>(&d_coor.z), 4);
    //in.read(reinterpret_cast<char*>(&d_xRot), 4);
    //in.read(reinterpret_cast<char*>(&d_yRot), 4);
  }

  ostream &operator<<(ostream &out, Drawable const &object)
  {
    object.v_insert(out);
    return out;
  }

  istream &operator>>(istream &in, Drawable &object)
  {
    object.v_extract(in);
    return in;
  }

}
