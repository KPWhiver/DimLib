// drawnode.cpp
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

#include "dim/drawnode.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

namespace dim
{
  mat4 DrawNodeBase::matrix() const
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

  DrawNodeBase::DrawNodeBase(vec3 const &coor, vec3 const &rot, float radius)
      : d_coor(coor), d_rot(rot), d_radius(radius)
  {
  }

  DrawNodeBase::~DrawNodeBase()
  {
  }

  glm::vec3 DrawNodeBase::coor() const
  {
    return d_coor;
  }

  void DrawNodeBase::setCoor(const vec3 & coor)
  {
    d_coor = coor;
  }

  glm::vec3 const &DrawNodeBase::rot() const
  {
    return d_rot;
  }

  void DrawNodeBase::draw()
  {
    shader().use();

    shader().set("in_mat_model", matrix());

    for(size_t mesh = 0; mesh != scene().size(); ++mesh)
    {
      for(size_t idx = 0; idx != scene()[mesh].textures().size(); ++idx)
        shader().set(scene()[mesh].textures()[idx].second, scene()[mesh].textures()[idx].first, idx);

      scene()[mesh].mesh().draw();
    }
  }

  void DrawNodeBase::insert(ostream &out) const
  {
	  out << d_coor.x << ' ' << d_coor.y << ' ' << d_coor.z << ' ' << d_rot.x << ' ' << d_rot.y << ' ' << d_rot.z << '\n';
    //out.write(reinterpret_cast<const char*>(&d_coor.x), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.y), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.z), 4);
    //out.write(reinterpret_cast<const char*>(&d_xRot), 4);
    //out.write(reinterpret_cast<const char*>(&d_yRot), 4);
  }

  void DrawNodeBase::extract(istream &in)
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

  ostream &operator<<(ostream &out, DrawNodeBase const &object)
  {
    object.insert(out);
    return out;
  }

  istream &operator>>(istream &in, DrawNodeBase &object)
  {
    object.extract(in);
    return in;
  }

}
