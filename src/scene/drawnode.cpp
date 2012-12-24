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

#include "dim/scene/drawnode.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

namespace dim
{
  NodeBase::NodeBase()
  :
      d_parent(0),
      d_scale(vec3(1.0)),
      d_modelMatrix(mat4(1.0)),
      d_changed(true)
  {
  }

  NodeBase::NodeBase(vec3 const &coor, quat const &orient, vec3 const &scale)
  :
      d_parent(0),
      d_coor(coor),
      d_orient(orient),
      d_scale(scale),
      d_modelMatrix(mat4(1.0)),
      d_changed(true)
  {
  }

  glm::vec3 NodeBase::location() const
  {
    return d_coor;
  }

  void NodeBase::setLocation(vec3 const &coor)
  {
    d_coor = coor;
    d_changed = true;
  }

  glm::quat const &NodeBase::orientation() const
  {
    return d_orient;
  }

  void NodeBase::setOrientation(quat const &orient)
  {
    d_orient = orient;
    d_changed = true;
  }

  glm::vec3 const &NodeBase::scaling() const
  {
    return d_scale;
  }

  void NodeBase::setScaling(vec3 const &scale)
  {
    d_scale = scale;
    d_changed = true;
  }

  void NodeBase::setChanged()
  {
    d_changed = true;
  }

  mat4 const &NodeBase::matrix()
  {
    if(d_changed)
    {
      d_modelMatrix = translate(mat4(1.0), d_coor);
      //if(d_rot.x != 0)
      //  d_modelMatrix = rotate(d_modelMatrix, d_rot.x, vec3(1.0, 0, 0));
      //if(d_rot.y != 0)
      //  d_modelMatrix = rotate(d_modelMatrix, d_rot.z, vec3(0, 0, 1.0));
      //if(d_rot.z != 0)
      //  d_modelMatrix = rotate(d_modelMatrix, d_rot.y, vec3(0, 1.0, 0));
      if(d_orient != quat(0, 0, 0, 1))
        d_modelMatrix *= mat4_cast(d_orient);

      if(d_scale != vec3(1.0))
        d_modelMatrix = scale(d_modelMatrix, d_scale);

      if(d_parent != 0)
        d_modelMatrix *= d_parent->matrix();

      d_changed = false;
    }

    return d_modelMatrix;
  }
  
  NodeBase * const NodeBase::parent()
  {
    return d_parent;
  }

  void NodeBase::setParent(NodeBase *parent)
  {
    d_parent = parent;
  }

  DrawNodeBase::DrawNodeBase(vec3 const &coor, quat const &orient, float radius)
      : NodeBase(coor, orient, vec3(1.0)),
        d_motionState(this)
  {
  }

  DrawNodeBase::DrawNodeBase()
      :
        d_motionState(this)
  {
  }

  DrawNodeBase::~DrawNodeBase()
  {
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
    out << location().x << ' ' << location().y << ' ' << location().z << ' ' << orientation().x << ' '
	      << orientation().y << ' ' << orientation().z << ' ' << orientation().w << '\n';

    //out.write(reinterpret_cast<const char*>(&d_coor.x), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.y), 4);
    //out.write(reinterpret_cast<const char*>(&d_coor.z), 4);
    //out.write(reinterpret_cast<const char*>(&d_xRot), 4);
    //out.write(reinterpret_cast<const char*>(&d_yRot), 4);
  }

  void DrawNodeBase::extract(istream &in)
  {
    vec3 coor;
    quat rot;
	  in >> coor.x >> coor.y >> coor.z >> rot.x >> rot.y >> rot.z >> rot.w;

	  coor/=100.0f;
	  rot/=100;

	  setLocation(coor);
	  setOrientation(rot);

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

  MotionState *DrawNodeBase::motionState()
  {
    return &d_motionState;
  }

  MotionState::MotionState(NodeBase *node)
  :
      d_node(node)
  {
  }

  void MotionState::getWorldTransform(btTransform &worldTransform) const
  {
    glm::quat const &orient = d_node->orientation();
    glm::vec3 const &coor = d_node->location();
    worldTransform = btTransform(btQuaternion(orient.x, orient.y, orient.z, orient.w), btVector3(coor.x, coor.y, coor.z));
  }

  void MotionState::setWorldTransform(btTransform const &worldTransform)
  {
    btQuaternion orient = worldTransform.getRotation();
    btVector3 coor = worldTransform.getOrigin();
    d_node->setOrientation(glm::quat(orient.w(), orient.x(), orient.y(), orient.z()));
    d_node->setLocation(glm::vec3(coor.x(), coor.y(), coor.z()));
    d_node->setChanged();
  }

}
