// drawable.hpp
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

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <iosfwd>
#include <string>

#include "dim/scene.hpp"
#include "dim/shader.hpp"
#include "dim/onepair.hpp"

namespace dim
{

  class Drawable
  {
      friend class SceneGraph;
      template <typename... RefType>
      friend class DrawableWrapper__;
      friend std::ostream &operator<<(std::ostream &out, Drawable const &object);
      friend std::istream &operator>>(std::istream &in, Drawable &object);

      glm::vec3 d_coor;
      glm::vec3 d_rot;

      float d_radius;

    public:
      typedef Onepair<long, 10000000> Key;

      Drawable() = default;
      Drawable(glm::vec3 const &coor, glm::vec3 const &rot, float radius);

      glm::vec3 coor() const;
      void setCoor(glm::vec3 const &coor);

      glm::vec3 const &rot() const;

      virtual Shader const &shader() const = 0;
      virtual Scene const &scene() const = 0;

      virtual void draw();

      virtual ~Drawable();

      virtual Drawable *clone() const = 0;

    protected:
      glm::mat4 matrix() const;
    
    private:
      virtual void insert(std::ostream &out) const;
      virtual void extract(std::istream &out);

  };



}

#endif /* DRAWABLE_HPP_ */
