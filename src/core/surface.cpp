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

#include "dim/core/surface.hpp"



namespace dim
{
  namespace internal
  {

    SurfaceBase* SurfaceBase::s_renderTarget(0);

    GLint SurfaceBase::s_maxAttachment(0);
  
    SurfaceBase::SurfaceBase()
    {
      static bool initialized = false;

      if(not initialized)
      {
        initialized = true;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &s_maxAttachment);

      }

    }

    SurfaceBase::~SurfaceBase()
    {
      if(s_renderTarget == this)
        s_renderTarget = 0;
    }

    Shader &SurfaceBase::copyShader()
    {
      static Shader copy(Shader::fromString, "copyShader", "", "#version 120\n"
                                       "uniform sampler2D in_texture;\n"
                                       "uniform vec2 in_viewport;\n"
                                       "void main()\n"
                                       "{gl_FragColor = texture2D(in_texture, gl_FragCoord.xy / in_viewport);}\n");
      return copy;
    }

    void SurfaceBase::finishRendering()
    {
    }

    void setViewport(int x, int y, int width, int height)
    {
      static int currentX(0);
      static int currentY(0);
      static int currentWidth(-1);
      static int currentHeight(-1);

      if(currentX != x || currentY != y || currentWidth != width || currentHeight != height)
      {
        glViewport(x, y, width, height);

        currentX = x;
        currentY = y;
        currentWidth = width;
        currentHeight = height;
      }
    }

    void setScissor(int x, int y, int width, int height)
    {
      static int currentX(0);
      static int currentY(0);
      static int currentWidth(-1);
      static int currentHeight(-1);

      if(currentX != x || currentY != y || currentWidth != width || currentHeight != height)
      {
        glScissor(x, y, width, height);

        currentX = x;
        currentY = y;
        currentWidth = width;
        currentHeight = height;
      }
    }

    void setBlending(bool blending)
    {
      static bool currentBlending(false);

      if(blending != currentBlending)
      {
        if(blending)
          glEnable(GL_BLEND);
        else
          glDisable(GL_BLEND);

        currentBlending = blending;
      }
    }
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

