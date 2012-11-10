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
  
  Surface<>::Surface(uint width, uint height)
  :
    d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;})
  {
    glGenFramebuffers(1, d_id.get());
    glBindFramebuffer(GL_FRAMEBUFFER, *d_id);
    if(GLEW_ARB_framebuffer_no_attachments)
    {
      glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
      glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
    }
    else
      log(__FILE__, __LINE__, LogType::note, "Framebuffers with no attachments are not supported");
      
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

