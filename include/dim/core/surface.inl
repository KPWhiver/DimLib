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
  namespace internal
  {
    struct Initializer
    {
      template<typename Type>
      void operator()(Type &item)
      {
        item = 0;
      }
    };
  }

  template<typename ...Types>
  Surface<Types...>::Surface(typename std::tuple_element<0, TuplePtrType>::type ptr)
      : 
        d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;}),
        d_colorAttachments(0), 
        d_depthComponent(false),
        d_colorComponent{false}, 
        d_minWidth(ptr->width()),
        d_minHeight(ptr->height()),
        d_clearDepth(1),
        d_blending(true)
  {
    forEach(d_targets, internal::Initializer{});

    glGenFramebuffers(1, d_id.get());
    addTarget<0>(ptr);
  }

  template<typename ...Types>
  Surface<Types...>::Surface(typename std::tuple_element<0, TupleType>::type obj)
      :
        d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;}),
        d_colorAttachments(0),
        d_depthComponent(false),
        d_colorComponent{false},
        d_minWidth(obj.width()),
        d_minHeight(obj.height()),
        d_clearDepth(1),
        d_blending(true)
  {
    forEach(d_targets, internal::Initializer{});

    std::get<0>(d_textures) = obj;
    glGenFramebuffers(1, d_id.get());
    addTarget<0>(&std::get<0>(d_textures));
  }

  template<typename ...Types>
  Surface<Types...>::Surface(uint width, uint height, Format format, Filtering filter)
      :
        d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;}),
        d_colorAttachments(0), 
        d_depthComponent(false),
        d_colorComponent{false}, 
        d_minWidth(width),
        d_minHeight(height),
        d_clearDepth(1),
        d_blending(true)
  { 
    forEach(d_targets, internal::Initializer{});

    glGenFramebuffers(1, d_id.get());
    addTarget<0>(format, filter);
  }

  template<typename ...Types>
  Surface<Types...>::Surface(uint width, uint height, NormalizedFormat format, Filtering filter)
      :
        d_id(new GLuint(0), [](GLuint *ptr){glDeleteFramebuffers(1, ptr); delete ptr;}),
        d_colorAttachments(0),
        d_depthComponent(false),
        d_colorComponent{false},
        d_minWidth(width),
        d_minHeight(height),
        d_clearDepth(1),
        d_blending(true)
  {
    forEach(d_targets, internal::Initializer{});

    glGenFramebuffers(1, d_id.get());
    addTarget<0>(format, filter);
  }
  
  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename Surface<Types...>::TupleType>::type &Surface<Types...>::texture()
  {
    if(std::get<Index>(d_targets) == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Trying to retrieve a not yet attached texture-target");

    return *std::get<Index>(d_targets);
  }
  
  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename Surface<Types...>::TupleType>::type const &Surface<Types...>::texture() const
  {
    if(std::get<Index>(d_targets) == 0)
      throw log(__FILE__, __LINE__, LogType::error, "Trying to retrieve a not yet attached texture-target");

    return *std::get<Index>(d_targets);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(typename std::tuple_element<Index, TupleType>::type obj)
  {
    std::get<Index>(d_textures) = obj;
    addTarget<Index>(&obj);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(typename std::tuple_element<Index, TuplePtrType>::type ptr)
  {
    ComponentType attachment = processFormat(ptr->externalFormat());

    addAttachment<Index>(attachment, ptr);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(Format format, Filtering filter)
  {
    // Create the texture
    typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;

    TextureType &tex = std::get<Index>(d_textures);
    tex = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

    ComponentType attachment = processFormat(tex.externalFormat());

    addAttachment<Index>(attachment, &tex);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addTarget(NormalizedFormat format, Filtering filter)
  {
     // Create the texture
     typedef typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type TextureType;

     TextureType &tex = std::get<Index>(d_textures);
     tex = TextureType(0, filter, format, width(), height(), false, Wrapping::borderClamp);

     ComponentType attachment = processFormat(tex.externalFormat());

     addAttachment<Index>(attachment, &tex);
  }

  template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::addAttachment(ComponentType attachment, typename std::tuple_element<Index, TuplePtrType>::type ptr)
  {
    if(ptr->width() < d_minWidth)
      d_minWidth = ptr->width();

    if(ptr->height() < d_minHeight)
      d_minHeight = ptr->height();

    std::get<Index>(d_targets) = ptr;

    glBindTexture(GL_TEXTURE_2D, ptr->id());

    // Set some texture coefficients
    if(attachment == depth)
      ptr->setBorderColor(glm::vec4(1.0f));

    glBindFramebuffer(GL_FRAMEBUFFER, *d_id);

    //glDrawBuffer(GL_NONE);

    // Add the texture to the FBO
    if(attachment == depth)
    {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ptr->id(), 0);
      d_attachments[Index] = GL_NONE;
    }
    else
    {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + d_colorAttachments, GL_TEXTURE_2D, ptr->id(), 0);
      d_attachments[Index] = GL_COLOR_ATTACHMENT0 + d_colorAttachments;
      ++d_colorAttachments;
    }

    // Check FBO
    GLenum fbo_status;
    fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    switch(fbo_status)
    {
      case GL_FRAMEBUFFER_COMPLETE:
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
      case GL_FRAMEBUFFER_UNSUPPORTED:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_UNSUPPORTED");
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
      default:
        throw log(__FILE__, __LINE__, LogType::error, "Creation of a framebuffer failed with error code: " + std::to_string(fbo_status));
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
        d_blending = false;
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
    return *d_id;
  }

  template<typename ...Types>
  uint Surface<Types...>::height() const
  {
    return d_minHeight;
  }

  template<typename ...Types>
  uint Surface<Types...>::width() const
  {
    return d_minWidth;
  }

  template<typename ...Types>
  void Surface<Types...>::setBlending(bool blending)
  {
    d_blending = blending;
  }

  template<typename ...Types>
  void Surface<Types...>::renderTo(bool clearBuffer)
  {
    renderToPart(0, 0, width(), height(), clearBuffer);
  }

  template<typename ...Types>
  void Surface<Types...>::renderTo(bool clearBuffer, bool drawBuffers[std::tuple_size<TupleType>::value])
  {
    renderToPart(0, 0, width(), height(), clearBuffer, drawBuffers);
  }

  template<typename ...Types>
  void Surface<Types...>::renderToPart(uint x, uint y, uint width, uint height, bool clearBuffer)
  {
    std::array<bool, std::tuple_size<TupleType>::value> drawBuffers;
    drawBuffers.fill(true);
    renderToPart(x, y, width, height, clearBuffer, drawBuffers);
  }

  template<typename ...Types>
  void Surface<Types...>::renderToPart(uint x, uint y, uint width, uint height, bool clearBuffer, bool drawBuffers[std::tuple_size<TupleType>::value])
  {    
    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(s_renderTarget != 0)
    {
      s_renderTarget->finishRendering();
      s_renderTarget = 0;
    }

    internal::setBlending(d_blending);

    internal::setViewport(0, 0, this->width(), this->height());
    internal::setScissor(x, y, width, height);

    if(clearBuffer)
      clear(drawBuffers);
    else // This is an optimisation (binding is done in clear)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, *d_id);

      GLuint buffers[std::tuple_size<TupleType>::value];
      for(size_t idx = 0; idx != std::tuple_size<TupleType>::value; ++idx)
      {
        if(drawBuffers[idx] == false)
        {
          buffers[idx] = GL_NONE;
          if(d_attachments[idx] == GL_NONE)
            glDepthMask(false);
        }
        else
        {
          buffers[idx] = d_attachments[idx];
          if(d_attachments[idx] == GL_NONE)
            glDepthMask(true);
        }
      }

      glDrawBuffers(std::tuple_size<TupleType>::value, buffers);
    }

    // Tell the attached textures that they might be changed
    notifyTextures();

    //glColorMask(d_colorComponent[0], d_colorComponent[1], d_colorComponent[2], d_colorComponent[3]);
    // Clear the buffer before drawing (this also binds it)

  }
  
  template<typename ...Types>
  template<typename Type, uint Index>
  void Surface<Types...>::copy(Texture<Type> const &source)
  {
    copyToPart(source, 0, 0, width(), height(), false);
  }
    
  template<typename ...Types>
  template<typename Type, uint Index>
  void Surface<Types...>::copyToPart(Texture<Type> const &source, uint x, uint y, uint width, uint height, bool clearBuffer)
  {
    std::array<bool, std::tuple_size<TupleType>::value> drawBuffers;
    drawBuffers[Index] = true;
    renderToPart(x, y, width, height, false, drawBuffers.data());
    
    internal::setBlending(false);
    
    copyShader().use();
    copyShader().set("in_viewport", glm::vec2(width, height));
    copyShader().set("in_texture", source, 0);
    
    drawFullscreenQuad();
  }

  
  template<typename ...Types>
  void Surface<Types...>::notifyTextures()
  {
    forEach(d_targets, internal::RenewBuffer{});
  }
  
  template<typename ...Types>
  void Surface<Types...>::clear()
  {
    std::array<bool, std::tuple_size<TupleType>::value> drawBuffers;
    drawBuffers.fill(true);
    clear(drawBuffers.data());
  }

  template<typename ...Types>
  void Surface<Types...>::clear(bool drawBuffers[std::tuple_size<TupleType>::value])
  {
    GLuint buffers[std::tuple_size<TupleType>::value];
    bool clearDepth = false;
    for(size_t idx = 0; idx != std::tuple_size<TupleType>::value; ++idx)
    {
      if(drawBuffers[idx] == false)
      {
        buffers[idx] = GL_NONE;
        if(d_attachments[idx] == GL_NONE)
          glDepthMask(false);
      }
      else
      {
        buffers[idx] = d_attachments[idx];
        if(d_attachments[idx] == GL_NONE)
        {
          glDepthMask(true);
          clearDepth = true;
        }
      }
    }

    clearDepth = clearDepth && d_depthComponent;

    glBindFramebuffer(GL_FRAMEBUFFER, *d_id);

    glDrawBuffers(std::tuple_size<TupleType>::value, buffers);

    if(d_clearDepth != 1 && clearDepth)
      glClearDepth(d_clearDepth);
    if(d_clearColor != glm::vec4() && d_colorComponent[0])
      glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, d_clearColor.a);

    if(clearDepth && d_colorComponent[0])
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else if(clearDepth)
      glClear(GL_DEPTH_BUFFER_BIT);
    else
      glClear(GL_COLOR_BUFFER_BIT);

    if(d_clearDepth != 1 && clearDepth)
      glClearDepth(1);
    if(d_clearColor != glm::vec4() && d_colorComponent[0])
      glClearColor(0, 0, 0, 0);
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

