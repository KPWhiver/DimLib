// windowsurface.cpp
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

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <stdexcept>

#include "dim/core/windowsurface.hpp"
#include "dim/core/surface.hpp"

using namespace std;
using namespace glm;

namespace dim
{
  WindowSurface::~WindowSurface()
  {
    d_destroyWindow();
  }

  size_t WindowSurface::height() const
  {
    return d_height;
  }

  size_t WindowSurface::width() const
  {
    return d_width;
  }

  void WindowSurface::setHeight(size_t newHeight)
  {
    d_height = newHeight;
  }

  void WindowSurface::setWidth(size_t newWidth)
  {
    d_width = newWidth;
  }

  void WindowSurface::setClearColor(vec4 const &color)
  {
    d_clearColor = color;
  }

  void WindowSurface::setClearDepth(float depth)
  {
    d_clearDepth = depth;
  }

  void WindowSurface::setBlending(bool blending)
  {
    d_blending = blending;
  }

  void WindowSurface::renderTo(bool clearBuffer)
  {
    renderToPart(0, 0, d_width, d_height, clearBuffer);
  }

  void WindowSurface::clear()
  {
    if(d_clearDepth != 0)
      glClearDepth(d_clearDepth);
    if(d_clearColor != glm::vec4())
      glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, d_clearColor.a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(d_clearDepth != 0)
      glClearDepth(0);
    if(d_clearColor != glm::vec4())
      glClearColor(0, 0, 0, 0);
  }

  void WindowSurface::renderToPart(size_t x, size_t y, size_t width, size_t height, bool clearBuffer)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    internal::setBlending(d_blending);

    internal::setViewport(0, 0, d_width, d_height);
    internal::setScissor(x, y, width, height);

    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(internal::SurfaceBase::s_renderTarget != 0)
      internal::SurfaceBase::s_renderTarget->finishRendering();

    internal::SurfaceBase::s_renderTarget = 0;

    // Clear the buffer before drawing
    if(clearBuffer)
      clear();

    glDepthMask(true);
    glColorMask(true, true, true, true);
    glDrawBuffer(GL_BACK);
  }

}

