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

#include <string>
#include <stdexcept>
#include <iostream>

#include "DIM/surface.hpp"
#include "DIM/font.hpp"

using namespace std;
using namespace glm;

namespace dim
{
  Surface::FrameBuffer::FrameBuffer()
    :
      d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;})
  {
  }

  Surface::Surface(size_t width, size_t height, Texture::Format format, bool pingPongBuffer, Texture::Filtering filter)
      : d_width(width), d_height(height), d_bufferToRenderTo(0), d_frames(1 + pingPongBuffer),
        d_colorBuffers(0), d_depthComponent(false), d_colorComponent{false}
  {
    Component attachment = processFormat(format);

    glGenFramebuffers(1, d_frames[0].d_id.get());
    addBuffer(attachment, 0, format, filter);

    if(pingPongBuffer)
    {
      glGenFramebuffers(1, d_frames[1].d_id.get());
      addBuffer(attachment, 1, format, filter);
    }

    if(attachment != depth)
      ++d_colorBuffers;
  }

  void Surface::addTarget(Texture::Format format, Texture::Filtering filter)
  {
    size_t oldDepth = d_depth;
    Component attachment = processFormat(format);

    if(oldDepth != d_depth)
    {
      d_depth = oldDepth;
      throw runtime_error(
          "Addition of a extra render target to a framebuffer failed because the depth doesn't match");
    }

    addBuffer(attachment, 0, format, filter);

    if(d_frames.size() == 2)
    {
      addBuffer(attachment, 1, format, filter);
    }

    if(attachment != depth)
      ++d_colorBuffers;
  }

  void Surface::addBuffer(Component attachment, size_t buffer, Texture::Format format, Texture::Filtering filter)
  {
    if(attachment == depth)
    {
      d_frames[buffer].d_texDepth = Texture(0, filter, format, d_width, d_height, Texture::borderClamp);
      glBindTexture(GL_TEXTURE_2D, d_frames[buffer].d_texDepth.id());
      d_frames[buffer].d_texDepth.setBorderColor(vec4(1.0f));
    }
    else
    {
      d_frames[buffer].d_texList[d_colorBuffers] = Texture(0, filter, format, d_width, d_height, Texture::borderClamp);
      glBindTexture(GL_TEXTURE_2D, d_frames[buffer].d_texList[d_colorBuffers].id());
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glBindFramebuffer(GL_FRAMEBUFFER, *d_frames[buffer].d_id);

    if(attachment == depth)
    {
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }

    if(attachment == depth)
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, d_frames[buffer].d_texDepth.id(), 0);
    else
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_frames[buffer].d_texList[0].id(), 0);
      //glFramebufferTexture2D(GL_FRAMEBUFFER, attachment + d_colorBuffers, GL_TEXTURE_2D, d_frames[buffer].d_texList[d_colorBuffers].id(), 0);

    GLenum fbo_status;
    fbo_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if(fbo_status != GL_FRAMEBUFFER_COMPLETE)
      throw runtime_error("Creation of a framebuffer failed");
  }

  Surface::Component Surface::processFormat(Texture::Format format)
  {
    // First pass only for depth
    switch(format)
    {
      case Format::R8:
        d_depth = 8;
        break;
      case Format::RG8:
      case Format::R16:
      case Format::D16:
        d_depth = 16;
        break;
      case Format::RGB8:
      case Format::sRGB8:
        d_depth = 24;
        break;
      case Format::RGBA8:
      case Format::sRGB8A8:
      case Format::RG16:
      case Format::R32:
      case Format::D32:
      case Format::R11G11B10:
        d_depth = 32;
        break;
      case Format::RGB16:
        d_depth = 48;
        break;
      case Format::RGBA16:
      case Format::RG32:
        d_depth = 64;
        break;
      case Format::RGB32:
        d_depth = 96;
        break;
      case Format::RGBA32:
        d_depth = 128;
        break;
    }

    // Second pass only for components
    switch(format)
    {
      case Format::RGBA8:
      case Format::sRGB8A8:
      case Format::RGBA16:
      case Format::RGBA32:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        d_colorComponent[3] = true;
        return color0;
      case Format::RGB8:
      case Format::sRGB8:
      case Format::RGB16:
      case Format::RGB32:
      case Format::R11G11B10:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color0;
      case Format::RG8:
      case Format::RG16:
      case Format::RG32:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        return color0;
      case Format::R8:
      case Format::R16:
      case Format::R32:
        d_colorComponent[0] = true;
        return color0;
      case Format::D16:
      case Format::D32:
        d_depthComponent = true;
        return depth;
    }
    return color0;
  }

  size_t Surface::height() const
  {
    return d_height;
  }

  size_t Surface::width() const
  {
    return d_width;
  }

  Texture const &Surface::texture(Component component) const
  {
    size_t idx = 0;
    if(d_frames.size() == 2 && d_bufferToRenderTo == 1)
      idx = 1;
  
    if(component == depth)
      return d_frames[idx].d_texDepth;
    
    return d_frames[idx].d_texList[component - color0];
  }

  void Surface::renderTo()
  {
    renderToPart(0, 0, d_width, d_height, true);
  }

  void Surface::renderToPart(size_t x, size_t y, size_t width, size_t height, bool clear)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, *d_frames[d_bufferToRenderTo].d_id);
    if(clear && d_colorComponent[4])
      glClearDepth(1.0);

    if(d_frames.size() == 2)
      d_bufferToRenderTo = ((d_bufferToRenderTo == 0) ? 1 : 0);

    glViewport(x, y, width, height);

    if(clear)
    {
      if(d_depthComponent && (d_colorComponent[0] || d_colorComponent[3]))
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      else if(d_depthComponent)
        glClear(GL_DEPTH_BUFFER_BIT);
      else
        glClear(GL_COLOR_BUFFER_BIT);
    }

    //glColorMask(d_colorComponent[0], d_colorComponent[1], d_colorComponent[2], d_colorComponent[3]);

  }

  GLuint Surface::id() const
  {
    return *d_frames[0].d_id;
  }

}

