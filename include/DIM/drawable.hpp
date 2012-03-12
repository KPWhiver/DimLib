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
#include <vector>

#include "DIM/drawstate.hpp"
#include "DIM/onepair.hpp"

namespace dim
{

  class Drawable
  {
      friend class DrawMap;
      template <typename... RefType>
      friend class DrawableWrapper__;
      friend std::ostream &operator<<(std::ostream &out, Drawable const &object);
      friend std::istream &operator>>(std::istream &in, Drawable &object);

      glm::vec3 d_coor;
      float d_xRot, d_yRot, d_radius;



    public:
      typedef Onepair<long, 10000000> Key;
      typedef std::pair<size_t, Key> Id;

      Drawable() = default;
      Drawable(glm::vec3 const &coor, float xRot, float yRot, float radius);

      Id id() const;

      glm::vec3 coor() const;
      void setCoor(glm::vec3 const &coor);

      float xRot() const;
      float yRot() const;

      std::vector<DrawState> const &drawStates() const;

      void draw(DrawState const &state);

      virtual ~Drawable();

    private:
      Id d_id;

      virtual void v_draw(DrawState const &state);
      virtual std::vector<DrawState> const &v_drawStates() const = 0;
      virtual void v_insert(std::ostream &out) const;
      virtual void v_extract(std::istream &out);

  };



}

#endif /* DRAWABLE_HPP_ */
