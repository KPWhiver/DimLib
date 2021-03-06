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

#include "dim/core/tools.hpp"
#include "dim/core/mesh.hpp"
#include "dim/core/shader.hpp"

namespace dim
{
  void drawFullscreenQuad()
  {
    static GLfloat data[12]{-1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1};
    static Mesh quad(data, 6, Shader::vertex, Shader::vec2);
    quad.draw();
  }
}
