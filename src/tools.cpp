// tools.cpp
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

#include "dim/tools.hpp"
#include "dim/mesh.hpp"

namespace dim
{
  void drawFullscreenQuad()
  {
    static bool initialized(false);

    static GLfloat data[6 * 2] {-1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1};
    static Mesh quad(data, {{Attribute::vertex, Attribute::vec2}}, 6, Mesh::triangle, 0);
    quad.draw();

    /*glEnableVertexAttribArray(0);
    quad.buffer().bind(Buffer<GLfloat>::data);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);*/
  }
}
