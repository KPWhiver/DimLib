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

#include "DIM/texture.hpp"

namespace dim
{
  template<typename ...Types>
  class Surface
  {
    //size_t d_width, d_height;
    size_t d_depth;

   	size_t d_bufferToRenderTo;

    struct FrameBuffer
    {
      std::tuple<Texture<Types>...> d_textures;
    
      //Texture d_texDepth;
      //Texture d_texList[4];
      std::shared_ptr<GLuint> d_id;

      FrameBuffer();
	};

    std::vector<FrameBuffer> d_frames;

    size_t d_colorBuffers;

    bool d_depthComponent;
    bool d_colorComponent[4];
    
    static Surface* lastRenderedTo;
    
    enum ComponentType
    {
      depth, 
      color, 
      stencil,
    };
    
  public:
    Surface(size_t width, size_t height, Texture::Format format, bool pingPongBuffer, Texture::Filtering filter = Texture::linear);
    
    template<int Index>
    void addTarget(Texture::Format format, Texture::Filtering filter = Texture::linear);

    size_t height() const;
    size_t width() const;

    template<int Index>
    std::tuple_element<Index, Texture<Types>...>::type &texture(Component component);

    void renderTo();
    void renderToPart(size_t x, size_t y, size_t width, size_t height, bool clear);

    GLuint id() const;

  private:
	Surface::ComponentType processFormat(Texture::Format format);
	void addBuffer(ComponentType attachment, size_t width, size_t height, size_t buffer, Texture::Format format, Texture::Filtering filter);
  };

  template<>
  class Surface
  {
    static_assert(true, "Error: Surface needs at least one template argument");
  };

}

#include "surface.inl";

#endif
