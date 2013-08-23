// drawnode.hpp
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

#ifndef DRAWNODE_HPP
#define DRAWNODE_HPP

#include <iosfwd>
#include <string>

#include "dim/scene/scene.hpp"
#include "dim/core/shader.hpp"
#include "dim/util/onepair.hpp"

#include <BulletDynamics/Dynamics/btRigidBody.h>

#include <glm/gtc/quaternion.hpp>

namespace dim
{
  class NodeBase;

  class MotionState : public btMotionState
  {
      NodeBase *d_node;
    public:
      MotionState(NodeBase *node);

    private:
      virtual void getWorldTransform(btTransform &worldTransform) const;

      virtual void setWorldTransform(btTransform const &worldTransform);
  };

  class NodeBase
  {
      template <typename ...Types>
      friend class SceneGraph;
      template <typename RefType>
      friend class NodeGrid;
      friend std::ostream &operator<<(std::ostream &out, NodeBase const &object);
      friend std::istream &operator>>(std::istream &in, NodeBase &object);

      MotionState d_motionState;

      NodeBase *d_parent;

      glm::vec3 d_coor;
      glm::quat d_orient;
      glm::vec3 d_scale;
      glm::mat4 d_modelMatrix;
      bool d_changed;

    public:
      NodeBase(glm::vec3 const &coor, glm::quat const &orient, glm::vec3 const &scale);
      NodeBase();

      virtual ~NodeBase();

      virtual NodeBase *clone() const = 0;

      virtual Shader const &shader(size_t idx) const = 0;
      virtual Scene const &scene() const = 0;

      virtual btRigidBody *rigidBody() = 0;

      virtual void updateNode(NodeBase *node, glm::vec3 const &from, glm::vec3 const &to){};

      glm::vec3 location() const;
      void setLocation(glm::vec3 const &coor);

      glm::quat const &orientation() const;
      void setOrientation(glm::quat const &orient);

      glm::vec3 const &scaling() const;
      void setScaling(glm::vec3 const &scale);

      glm::mat4 const &matrix();

      MotionState *motionState();

      void setChanged();

      virtual void draw();

    protected:
      void setParent(NodeBase *parent);
      NodeBase * const parent();

      virtual void insert(std::ostream &out) const;
      virtual void extract(std::istream &in);
  };



  /*class DrawNodeBase : public NodeBase
  {
    public:
      DrawNodeBase();
      DrawNodeBase(glm::vec3 const &coor, glm::quat const &orient, float radius);

      virtual Shader const &shader() const = 0;
      virtual Scene const &scene() const = 0;

      virtual btRigidBody *rigidBody() = 0;



      virtual ~DrawNodeBase();

      virtual DrawNodeBase *clone() const = 0;
  };*/

  namespace internal
  {
    class DefaultNode : public NodeBase
    {
      static Scene s_defaultScene;
      public:
        virtual Shader const &shader(size_t idx) const
        {
          return Shader::defaultShader();
        }

        virtual Scene const &scene() const
        {
          return s_defaultScene;
        }

        virtual btRigidBody *rigidBody()
        {
          return 0;
        }

        virtual NodeBase *clone() const
        {
          return new DefaultNode();
        }
    };
  }
}

#endif /* DRAWNODE_HPP_ */
