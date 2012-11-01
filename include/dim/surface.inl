// surface.inl
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

namespace dim
{
  
  template<typename ...Types>
  Surface<Types...>::FrameBuffer::FrameBuffer()
    :
      d_targets{0},
      d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;})
  {
  }

  template<typename ...Types>
  Surface<Types...>::Surface(std::tuple_element<0, TuplePtrType>::type ptr, bool pingPingBuffer)
      : 
        d_buffers(1 + pingPongBuffer),
        d_colorAttachments(0), 
        d_depthComponent(false),
        d_colorComponent{false}, 
        d_minWidth(ptr->width()),
        d_minHeight(ptr->height()),
        d_clearDepth(0)
  {
    
  
  }

  template<typename ...Types>
  Surface<Types...>::Surface(uint width, uint height, Format format, bool pingPongBuffer, Filtering filter)
      : 
        d_buffers(1 + pingPongBuffer),
        d_colorAttachments(0), 
        d_depthComponent(false),
        d_colorComponent{false}, 
        d_minWidth(width),
        d_minHeight(height),
        d_clearDepth(0)
  { 
    ComponentType attachment = processFormat(format);
    
    glGenFramebuffers(1, d_buffers[0].d_id.get());
    addAttachment<0>(attachment, width, height, 0, format, filter);

    if(attachment == depth)
      d_attachments[0] = GL_NONE;
    else
      d_attachments[0] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;
    
    if(pingPongBuffer)
    {
      glGenFramebuffers(1, d_buffers[1].d_id.get());
      addAttachment<0>(attachment, width, height, 1, format, filter);
    }

    if(attachment == color)
      ++d_colorAttachments;
  }
  
  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename Surface<Types...>::TupleType>::type &Surface<Types...>::texture()
  {
    uint idx = 0;
    if(d_buffers.size() == 2 && d_bufferToRenderTo == 0)
      idx = 1;

    return *std::get<Index>(d_buffers[idx].d_targets);
  }


  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(Format format, Filtering filter)
  {
    //uint oldDepth = d_depth;
    ComponentType attachment = processFormat(format);

    //if(oldDepth != d_depth)
    //{
    //  d_depth = oldDepth;
    //  log(__FILE__, __LINE__, LogType::error, "Addition of a extra render target to a framebuffer failed because the depth doesn't match");
    //}
    
    addAttachment<Index>(attachment, width(), height(), 0, format, filter);
    
    if(attachment == depth)
      d_attachments[Index] = GL_NONE;
    else
      d_attachments[Index] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;

    if(d_buffers.size() == 2)
      addAttachment<Index>(attachment, width(), height(), 1, format, filter);

    if(attachment == color)
      ++d_colorAttachments;
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addAttachment(ComponentType attachment, uint width, uint height, uint buffer, Format format, Filtering filter)
  {
    if(width < d_minWidth)
      d_minWidth = width;
  
    if(height < d_minHeight)
      d_minHeight = height;
  
    // Create the texture
    typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;
    
    TextureType &tex = std::get<Index>(d_buffers[buffer].d_textures);
    tex = TextureType(0, filter, format, width, height, false, Wrapping::borderClamp);
    
    std::get<Index>(d_buffers[buffer].d_targets) = &tex;
    
    
    glBindTexture(GL_TEXTURE_2D, tex.id());
    
    // Set some texture coefficients
    if(attachment == depth)
      tex.setBorderColor(glm::vec4(1.0f));
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glBindFramebuffer(GL_FRAMEBUFFER, *d_buffers[buffer].d_id);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Add the texture to the FBO
    if(attachment == depth)
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.id(), 0);
    else
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + d_colorAttachments, GL_TEXTURE_2D, tex.id(), 0);
    
    // Check FBO
    GLenum fbo_status;
    fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if(fbo_status != GL_FRAMEBUFFER_COMPLETE)
    {
      std::stringstream ss;
      ss << "Creation of a framebuffer failed with error code: " << fbo_status;
      log(__FILE__, __LINE__, LogType::error, ss.str());
    }
  }

  template<typename ...Types>
  typename Surface<Types...>::ComponentType Surface<Types...>::processFormat(Format format)
  {
    switch(format)
    {
      case Format::R11G11B10:
        //d_depth = 32;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color;
      
      case Format::RGBA8:
      case Format::sRGB8A8:
        //d_depth = 32;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        d_colorComponent[3] = true;
        return color;
      case Format::RGB8:
      case Format::sRGB8:
        //d_depth = 24;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color;
      case Format::RG8:
        //d_depth = 16;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        return color;
      case Format::R8:
        //d_depth = 8;
        d_colorComponent[0] = true;
        return color;

      case Format::RGBA16:
        //d_depth = 64;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        d_colorComponent[3] = true;
        return color;
      case Format::RGB16:
        //d_depth = 48;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color;
      case Format::RG16:
        //d_depth = 32;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        return color;
      case Format::R16:
        //d_depth = 16;
        d_colorComponent[0] = true;
        return color;
      case Format::D16:
        //d_depth = 16;
        d_depthComponent = true;
        return depth;

      case Format::RGBA32:
        //d_depth = 128;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        d_colorComponent[3] = true;
        return color;
      case Format::RGB32:
        //d_depth = 96;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color;
      case Format::RG32:
        //d_depth = 64;
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        return color;
      case Format::R32:
        //d_depth = 32;
        d_colorComponent[0] = true;
        return color;
      case Format::D32:
        //d_depth = 32;
        d_depthComponent = true;
        return depth;
    }
    return color;
  }

  template<typename ...Types>
  GLuint Surface<Types...>::id() const
  {
    return *d_buffers[0].d_id;
  }

  template<typename ...Types>
  uint Surface<Types...>::height() const
  {
    //return std::get<0>(d_buffers[0].d_textures).height();
    return minHeight;
  }

  template<typename ...Types>
  uint Surface<Types...>::width() const
  {
    //return std::get<0>(d_buffers[0].d_textures).width();
    return minWidth;
  }

  template<typename ...Types>
  void Surface<Types...>::renderTo(bool clear, bool swapBuffers)
  {
    renderToPart(0, 0, width(), height(), clear, swapBuffers);
  }

  template<typename ...Types>
  void Surface<Types...>::renderToPart(uint x, uint y, uint width, uint height, bool clear, bool swapBuffers)
  {    
    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(s_renderTarget != 0)
      s_renderTarget->swapBuffers();
    
    if(swapBuffers)
      s_renderTarget = this;
    else
      s_renderTarget = 0;
    

    glBindFramebuffer(GL_FRAMEBUFFER, *d_buffers[d_bufferToRenderTo].d_id);

    // TODO change this to glDrawBuffers
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    //glDrawBuffers(d_colorAttachments - 1, d_attachments);

    glViewport(x, y, width, height);
    
    // Clear the buffer before drawing
    if(clear)
    {        
      if(d_clearDepth != 0 && d_depthComponent)
        glClearDepth(d_clearDepth);
      if(d_clearColor != glm::vec4() && (d_colorComponent[0] || d_colorComponent[3]))
        glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, d_clearColor.a);
      
      if(d_depthComponent && (d_colorComponent[0] || d_colorComponent[3]))
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      else if(d_depthComponent)
        glClear(GL_DEPTH_BUFFER_BIT);
      else
        glClear(GL_COLOR_BUFFER_BIT);    
      
      if(d_clearDepth != 0 && d_depthComponent)
        glClearDepth(0);
      if(d_clearColor != glm::vec4() && (d_colorComponent[0] || d_colorComponent[3]))
        glClearColor(0, 0, 0, 0);
    }
    
    // Tell the attached textures that they might be changed
    if(d_buffers.size() == 2)
      notifyTextures(d_bufferToRenderTo);
    else
      notifyTextures(0);

    //glColorMask(d_colorComponent[0], d_colorComponent[1], d_colorComponent[2], d_colorComponent[3]);

  }
  
  template<typename ...Types>
  void Surface<Types...>::notifyTextures(uint buffer)
  {
    TupleCaller<std::tuple_size<TupleType>::value - 1, TupleType> call(d_buffers[buffer].d_textures);
  }
  
  template<typename ...Types>
  void Surface<Types...>::swapBuffers()
  {
    if(d_buffers.size() == 2)
      d_bufferToRenderTo = ((d_bufferToRenderTo == 0) ? 1 : 0);
  }

  template<typename ...Types>
  void Surface<Types...>::clear()
  {
    renderTo();
    renderTo();
  }
  
  template<typename ...Types>
  void Surface<Types...>::setClearColor(glm::vec4 const &color)
  {
    d_clearColor = color;
  }
  
  template<typename ...Types>
  void Surface<Types...>::setClearDepth(float depth)
  {
    d_clearDepth = depth;
  }
}

