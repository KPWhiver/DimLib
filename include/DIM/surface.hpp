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

#include "DIM/texture.hpp"

namespace dim
{
  class SurfaceBase__
  {
    friend class Window;

    protected:
      static SurfaceBase__* s_renderTarget;

      uint d_bufferToRenderTo;

      SurfaceBase__();
      virtual ~SurfaceBase__();

    public:
      // TODO fix this mess (this is supposed to be private)
      virtual void swapBuffers() = 0;
  };


  template<typename ...Types>
  class Surface : public SurfaceBase__
  {
    uint d_depth;

    struct FrameBuffer
    {
      std::tuple<Texture<Types>...> d_textures;
    
      std::shared_ptr<GLuint> d_id;

      FrameBuffer();
	  };

    std::vector<FrameBuffer> d_frames;

    uint d_colorBuffers;

    bool d_depthComponent;
    bool d_colorComponent[4];
    
    glm::vec4 d_clearColor;

    enum ComponentType
    {
      depth, 
      color, 
      stencil,
    };
    
  public:
    Surface(uint width, uint height, Format format, bool pingPongBuffer, Filtering filter = Filtering::linear);
    
    template<uint Index>
    void addTarget(Format format, Filtering filter = Filtering::linear);

    uint height() const;
    uint width() const;

    template<uint Index = 0>
    typename std::tuple_element<Index, std::tuple<Texture<Types>...>>::type &texture();

    void setClearColor(glm::vec4 const &color);
    void clear();

    void renderTo();
    void renderToPart(uint x, uint y, uint width, uint height, bool clear);

    GLuint id() const;

  private:
	  Surface::ComponentType processFormat(Format format);
	  
	  virtual void swapBuffers();

    template<uint Index>
    void addBuffer(ComponentType attachment, uint width, uint height, uint buffer, Format format, Filtering filter);
  };

  template<>
  class Surface<>
  {
    static_assert(true, "Error: Surface needs at least one template argument");
  };

}

#include "surface.inl"

#endif
