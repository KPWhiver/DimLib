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

#include "dim/scene.hpp"
#include "dim/shader.hpp"
#include "dim/onepair.hpp"

namespace dim
{
  class NodeBase
  {
      NodeBase *d_parent;

      glm::vec3 d_coor;
      glm::vec3 d_rot;
      glm::vec3 d_scale;

      glm::mat4 d_modelMatrix;

      bool d_changed;

    public:
      NodeBase(glm::vec3 const &coor, glm::vec3 const &rot, glm::vec3 const &scale);
      NodeBase();

      glm::vec3 coor() const;
      void setCoor(glm::vec3 const &coor);

      glm::vec3 const &rotation() const;
      void setRotation(glm::vec3 const &rot);

      glm::vec3 const &scaling() const;
      void setScaling(glm::vec3 const &scale);

      glm::mat4 const &matrix();

      void setChanged();
      
    protected:
      void setParent(NodeBase *parent);
      NodeBase * const parent();
  };

  class DrawNodeBase : public NodeBase
  {
      friend class SceneGraph;
      template <typename RefType>
      friend class NodeGrid;
      friend std::ostream &operator<<(std::ostream &out, DrawNodeBase const &object);
      friend std::istream &operator>>(std::istream &in, DrawNodeBase &object);

      float d_radius;

    public:
      DrawNodeBase();
      DrawNodeBase(glm::vec3 const &coor, glm::vec3 const &rot, float radius);

      virtual Shader const &shader() const = 0;
      virtual Scene const &scene() const = 0;

      virtual void draw();

      virtual ~DrawNodeBase();

      virtual DrawNodeBase *clone() const = 0;
    
    private:
      virtual void insert(std::ostream &out) const;
      virtual void extract(std::istream &in);

  };

  namespace internal
  {
    class DefaultDrawNode : public DrawNodeBase
    {
      static Scene s_defaultScene;
      public:
        virtual Shader const &shader() const
        {
          return Shader::defaultShader();
        }

        virtual Scene const &scene() const
        {

          return s_defaultScene;
        }

        virtual DrawNodeBase *clone() const
        {
          return new DefaultDrawNode();
        }
    };
  }
}

#endif /* DRAWNODE_HPP_ */
