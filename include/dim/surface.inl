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
  Surface<Types...>::Surface(typename std::tuple_element<0, TuplePtrType>::type ptr, bool pingPongBuffer)
      : 
        d_buffers(1 + pingPongBuffer),
        d_colorAttachments(0), 
        d_depthComponent(false),
        d_colorComponent{false}, 
        d_minWidth(ptr->width()),
        d_minHeight(ptr->height()),
        d_clearDepth(0)
  {
    glGenFramebuffers(1, d_buffers[0].d_id.get());
    
    if(pingPongBuffer)
      glGenFramebuffers(1, d_buffers[1].d_id.get());

    addTarget<0>(ptr);
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
    glGenFramebuffers(1, d_buffers[0].d_id.get());
    
    if(pingPongBuffer)
      glGenFramebuffers(1, d_buffers[1].d_id.get());

    addTarget<0>(format, filter);
  }

  template<typename ...Types>
  Surface<Types...>::Surface(uint width, uint height, NormalizedFormat format, bool pingPongBuffer, Filtering filter)
      :
        d_buffers(1 + pingPongBuffer),
        d_colorAttachments(0),
        d_depthComponent(false),
        d_colorComponent{false},
        d_minWidth(width),
        d_minHeight(height),
        d_clearDepth(0)
  {
    glGenFramebuffers(1, d_buffers[0].d_id.get());

    if(pingPongBuffer)
      glGenFramebuffers(1, d_buffers[1].d_id.get());

    addTarget<0>(format, filter);
  }
  
  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename Surface<Types...>::TupleType>::type &Surface<Types...>::texture()
  {
    uint idx = 0;
    if(d_buffers.size() == 2 && d_bufferToRenderTo == 0)
      idx = 1;

    if(std::get<Index>(d_buffers[idx].d_targets) == 0)
      log(__FILE__, __LINE__, LogType::error, "Trying to retrieve a not yet attached texture-target");

    return *std::get<Index>(d_buffers[idx].d_targets);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(typename std::tuple_element<Index, TuplePtrType>::type ptr)
  {
    ComponentType attachment = processFormat(ptr->externalFormat());

    addAttachment<Index>(attachment, 0, ptr);

    if(attachment == depth)
      d_attachments[Index] = GL_NONE;
    else
      d_attachments[Index] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;

    if(d_buffers.size() == 2)
    {
      typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;
      TextureType &tex = std::get<Index>(d_buffers[1].d_textures);
      tex = ptr->copy();
      addAttachment<Index>(attachment, 1, &tex);
    }

    if(attachment == color)
      ++d_colorAttachments;
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(Format format, Filtering filter)
  {
    // Create the texture
    typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;

    TextureType &tex = std::get<Index>(d_buffers[0].d_textures);
    tex = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

    ComponentType attachment = processFormat(tex.externalFormat());

    addAttachment<Index>(attachment, 0, &tex);
    
    if(attachment == depth)
      d_attachments[Index] = GL_NONE;
    else
      d_attachments[Index] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;

    if(d_buffers.size() == 2)
    {
      TextureType &tex2 = std::get<Index>(d_buffers[0].d_textures);
      tex2 = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

      addAttachment<Index>(attachment, 1, &tex2);
    }

    if(attachment == color)
      ++d_colorAttachments;
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(NormalizedFormat format, Filtering filter)
  {
    // Create the texture
     typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;

     TextureType &tex = std::get<Index>(d_buffers[0].d_textures);
     tex = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

     ComponentType attachment = processFormat(tex.externalFormat());

     addAttachment<Index>(attachment, 0, &tex);

     if(attachment == depth)
       d_attachments[Index] = GL_NONE;
     else
       d_attachments[Index] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;

     if(d_buffers.size() == 2)
     {
       TextureType &tex2 = std::get<Index>(d_buffers[0].d_textures);
       tex2 = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

       addAttachment<Index>(attachment, 1, &tex2);
     }

     if(attachment == color)
       ++d_colorAttachments;
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addAttachment(ComponentType attachment, uint buffer, typename std::tuple_element<Index, TuplePtrType>::type ptr)
  {
    if(ptr->width() < d_minWidth)
      d_minWidth = ptr->width();

    if(ptr->height() < d_minHeight)
      d_minHeight = ptr->height();

    std::get<Index>(d_buffers[buffer].d_targets) = ptr;

    glBindTexture(GL_TEXTURE_2D, ptr->id());

    // Set some texture coefficients
    if(attachment == depth)
      ptr->setBorderColor(glm::vec4(1.0f));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glBindFramebuffer(GL_FRAMEBUFFER, *d_buffers[buffer].d_id);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Add the texture to the FBO
    if(attachment == depth)
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ptr->id(), 0);
    else
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + d_colorAttachments, GL_TEXTURE_2D, ptr->id(), 0);

    // Check FBO
    GLenum fbo_status;
    fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    switch(fbo_status)
    {
      case GL_FRAMEBUFFER_COMPLETE:
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_UNSUPPORTED");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        break;
      default:
        std::stringstream ss;
        ss << "Creation of a framebuffer failed with error code: " << fbo_status;
        log(__FILE__, __LINE__, LogType::error, ss.str());
        break;
    }
  }

  /*template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::removeTarget()
  {

  }*/

  template<typename ...Types>
  typename Surface<Types...>::ComponentType Surface<Types...>::processFormat(GLuint format)
  {
    switch(format)
    {
      case GL_RGBA:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        d_colorComponent[3] = true;
        return color;
      case GL_RGB:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        d_colorComponent[2] = true;
        return color;
      case GL_RG:
        d_colorComponent[0] = true;
        d_colorComponent[1] = true;
        return color;
      case GL_RED:
        d_colorComponent[0] = true;
        return color;
      case GL_DEPTH_COMPONENT:
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
    return d_minHeight;
  }

  template<typename ...Types>
  uint Surface<Types...>::width() const
  {
    //return std::get<0>(d_buffers[0].d_textures).width();
    return d_minWidth;
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
    TupleCaller<std::tuple_size<TuplePtrType>::value - 1, TuplePtrType> call(d_buffers[buffer].d_targets);
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

