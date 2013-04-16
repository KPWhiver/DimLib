// doublebufferedsurface.hpp
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

#ifndef DOUBLEBUFFEREDSURFACE_HPP
#define DOUBLEBUFFEREDSURFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <stdexcept>

#include "dim/core/surface.hpp"

namespace dim
{
  template<typename ...Types>
  class DoubleBufferedSurface : public internal::SurfaceBase
  {
    typedef std::tuple<Texture<Types>...> TupleType;
    typedef std::tuple<Texture<Types>*...> TuplePtrType;

    Surface<Types...> d_buffer[2];

    uint d_bufferToRenderTo;

  public:
    DoubleBufferedSurface(uint width, uint height, Format format, Filtering filter = Filtering::linear);
    DoubleBufferedSurface(uint width, uint height, NormalizedFormat format, Filtering filter = Filtering::linear);
    explicit DoubleBufferedSurface(typename std::tuple_element<0, TuplePtrType>::type ptr);

    DoubleBufferedSurface(DoubleBufferedSurface const &other) = delete;
    DoubleBufferedSurface(DoubleBufferedSurface &&tmp) = default;

    DoubleBufferedSurface& operator=(DoubleBufferedSurface const &other) = delete;
    DoubleBufferedSurface& operator=(DoubleBufferedSurface &&tmp) = default;
    
    template<uint Index>
    void addTarget(Format format, Filtering filter = Filtering::linear);
    
    template<uint Index>
    void addTarget(NormalizedFormat format, Filtering filter = Filtering::linear);

    template<uint Index>
    void addTarget(typename std::tuple_element<Index, TuplePtrType>::type ptr);
    
    //template<uint Index>
    //void removeTarget();

    uint height() const;
    uint width() const;

    template<uint Index = 0>
    typename std::tuple_element<Index, TupleType>::type &texture();
    
    template<uint Index = 0>
    typename std::tuple_element<Index, TupleType>::type const &texture() const;

    void setClearColor(glm::vec4 const &color);
    void setClearDepth(float depth);

    void clear();

    void swapBuffers();

    void renderTo(bool clearBuffer = true, bool swapBuffer = true);
    void renderToPart(uint x, uint y, uint width, uint height, bool clearBuffer, bool swapBuffer);
    
    template<typename Type, uint Index = 0>
    void copy(Texture<Type> const &source);
    
    template<typename Type, uint Index = 0>
    void copyPart(Texture<Type> const &source, uint x, uint y, uint width, uint height);

    GLuint id() const;

  private:
    virtual void finishRendering();
  };

  template<>
  class DoubleBufferedSurface<>;
}

#include "doublebufferedsurface.inl"

#endif
