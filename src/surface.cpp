// surface.cpp
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

#include "dim/surface.hpp"

namespace dim
{
  SurfaceBase__* SurfaceBase__::s_renderTarget(0);
  
  GLint SurfaceBase__::s_maxAttachment(0);

  SurfaceBase__::SurfaceBase__()
  :
      d_bufferToRenderTo(0)
  {
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &s_maxAttachment);
  }
  
  SurfaceBase__::~SurfaceBase__()
  {
    if(s_renderTarget == this)
      s_renderTarget = 0;
  }
}

