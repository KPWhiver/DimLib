// surface.hpp
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

#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <stdexcept>

#include "dim/texture.hpp"

namespace dim
{
  class SurfaceBase__
  {
    friend class Window;

    protected:
      static SurfaceBase__* s_renderTarget;

      static GLint s_maxAttachment;

      uint d_bufferToRenderTo;

      SurfaceBase__();
      virtual ~SurfaceBase__();

      // TODO add initialize

    public:
      virtual void swapBuffers() = 0;
  };


  template<typename ...Types>
  class Surface : public SurfaceBase__
  {
    typedef std::tuple<Texture<Types>...> TupleType;

    uint d_depth;

    struct FrameBuffer
    {
      TupleType d_textures;

      std::shared_ptr<GLuint> d_id;

      FrameBuffer();
	  };

    GLuint d_attachments[std::tuple_size<TupleType>::value];

    std::vector<FrameBuffer> d_buffers;

    uint d_colorAttachments;

    bool d_depthComponent;
    bool d_colorComponent[4];
    
    glm::vec4 d_clearColor;
    float d_clearDepth;

    enum ComponentType
    {
      depth, 
      color, 
      stencil,
    };
    
  public:
    Surface(uint width, uint height, Format format, bool pingPongBuffer, Filtering filter = Filtering::linear);
    Surface(Surface const &other) = delete;

    Surface(Surface &&tmp) = default;

    Surface& operator=(Surface const &other) = delete;
    Surface& operator=(Surface &&tmp) = default;
    
    template<uint Index>
    void addTarget(Format format, Filtering filter = Filtering::linear);

    uint height() const;
    uint width() const;

    template<uint Index = 0>
    typename std::tuple_element<Index, TupleType>::type &texture();

    void setClearColor(glm::vec4 const &color);
    void setClearDepth(float depth);

    void clear();
    virtual void swapBuffers();

    void renderTo();
    void renderToPart(uint x, uint y, uint width, uint height, bool clear, bool swapBuffers);

    GLuint id() const;

  private:
	  Surface::ComponentType processFormat(Format format);

	  void notifyTextures(uint buffer);

    template<uint Index>
    void addAttachment(ComponentType attachment, uint width, uint height, uint buffer, Format format, Filtering filter);

    //TupleCaller class
    template<uint Index, typename TupleCallType>
    class TupleCaller
    {
      TupleCaller<Index - 1, TupleCallType> d_next;

      public:
      TupleCaller(TupleCallType &tuple);
    };

    template<typename TupleCallType>
    class TupleCaller<0, TupleCallType>
    {
      public:
      TupleCaller(TupleCallType &tuple);
    };
    //--
  };

  template<>
  class Surface<>
  {
    static_assert(true, "Error: Surface needs at least one template argument");
  };

  template<typename ...Types>
  template<uint Index, typename TupleCallType>
  Surface<Types...>::TupleCaller<Index, TupleCallType>::TupleCaller(TupleCallType &tuple)
  :
    d_next(tuple)
  {
    std::get<Index>(tuple).renewBuffer();
  }

  template<typename ...Types>
  template<typename TupleCallType>
  Surface<Types...>::TupleCaller<0, TupleCallType>::TupleCaller(TupleCallType &tuple)
  {
    std::get<0>(tuple).renewBuffer();
  }
}

#include "surface.inl"

#endif
