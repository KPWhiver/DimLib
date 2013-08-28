// doublebufferedsurface.inl
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

#include <iostream>

namespace dim
{
  template<typename ...Types>
  DoubleBufferedSurface<Types...>::DoubleBufferedSurface(typename std::tuple_element<0, TuplePtrType>::type ptr)
      : 
        d_buffer{Surface<Types...>(ptr->copy()), Surface<Types...>(ptr)},
        d_bufferToRenderTo(0)
  {
  }

  template<typename ...Types>
  DoubleBufferedSurface<Types...>::DoubleBufferedSurface(uint width, uint height, Format format, Filtering filter)
      : 
        d_buffer{Surface<Types...>(width, height, format, filter), Surface<Types...>(width, height, format, filter)},
        d_bufferToRenderTo(0)
  { 
  }

  template<typename ...Types>
  DoubleBufferedSurface<Types...>::DoubleBufferedSurface(uint width, uint height, NormalizedFormat format, Filtering filter)
      :
        d_buffer{Surface<Types...>(width, height, format, filter), Surface<Types...>(width, height, format, filter)},
        d_bufferToRenderTo(0)
  {
  }
  
  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename DoubleBufferedSurface<Types...>::TupleType>::type &DoubleBufferedSurface<Types...>::texture()
  {
    return d_buffer[!d_bufferToRenderTo].texture<Index>();
  }

  template<typename ...Types>
  template<uint Index>
  typename std::tuple_element<Index, typename DoubleBufferedSurface<Types...>::TupleType>::type const &DoubleBufferedSurface<Types...>::texture() const
  {
    return d_buffer[!d_bufferToRenderTo].texture<Index>();
  }

  template<typename ...Types>
  template<uint Index>
  void DoubleBufferedSurface<Types...>::addTarget(typename std::tuple_element<Index, TuplePtrType>::type ptr)
  {
    d_buffer[0].addTarget<Index>(ptr->copy());
    d_buffer[1].addTarget<Index>(ptr);
  }

  template<typename ...Types>
  template<uint Index>
  void DoubleBufferedSurface<Types...>::addTarget(Format format, Filtering filter)
  {
    d_buffer[0].addTarget<Index>(format, filter);
    d_buffer[1].addTarget<Index>(format, filter);
  }

  template<typename ...Types>
  template<uint Index>
  void DoubleBufferedSurface<Types...>::addTarget(NormalizedFormat format, Filtering filter)
  {
    d_buffer[0].addTarget<Index>(format, filter);
    d_buffer[1].addTarget<Index>(format, filter);
  }

  /*template<typename ...Types>
  template<uint Index>
  void Surface<Types...>::removeTarget()
  {

  }*/

  template<typename ...Types>
  GLuint DoubleBufferedSurface<Types...>::id() const
  {
    return d_buffer[d_bufferToRenderTo].id();
  }

  template<typename ...Types>
  uint DoubleBufferedSurface<Types...>::height() const
  {
    return d_buffer[0].height();
  }

  template<typename ...Types>
  uint DoubleBufferedSurface<Types...>::width() const
  {
    return d_buffer[0].width();
  }

  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::renderTo(bool clearBuffer, bool swapBuffer)
  {
    renderToPart(0, 0, width(), height(), clearBuffer, swapBuffer);
  }

  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::renderToPart(uint x, uint y, uint width, uint height, bool clearBuffer, bool swapBuffer)
  {    
    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(s_renderTarget != 0)
      s_renderTarget->finishRendering();

    s_renderTarget = 0;

    d_buffer[d_bufferToRenderTo].renderToPart(x, y, width, height, clearBuffer);

    if(swapBuffer)
      s_renderTarget = this;
  }
    
  template<typename ...Types>
  template<typename Type, uint Index>
  void DoubleBufferedSurface<Types...>::copy(Texture<Type> const &source, bool swapBuffers)
  {
    copyToPart(source, 0, 0, width(), height(), false, swapBuffers);
  }

  template<typename ...Types>
  template<typename Type, uint Index>
  void DoubleBufferedSurface<Types...>::copyToPart(Texture<Type> const &source, uint x, uint y, uint width, uint height, bool clearBuffer, bool swapBuffers)
  {
    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(s_renderTarget != 0)
      s_renderTarget->finishRendering();

    s_renderTarget = 0;

    d_buffer[d_bufferToRenderTo].copyToPart(source, x, y, width, height, clearBuffer);

    if(swapBuffers)
      s_renderTarget = this;
  }
  
  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::finishRendering()
  {
    swapBuffers();
  }

  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::swapBuffers()
  {
    // 0 --> 1, 1 --> 0
    d_bufferToRenderTo = !d_bufferToRenderTo;
  }

  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::clear()
  {
    d_buffer[d_bufferToRenderTo].clear();
  }
  
  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::setClearColor(glm::vec4 const &color)
  {
    d_buffer[0].setClearColor(color);
    d_buffer[1].setClearColor(color);
  }
  
  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::setClearDepth(float depth)
  {
    d_buffer[0].setClearDepth(depth);
    d_buffer[1].setCleardepth(depth);
  }

  template<typename ...Types>
  void DoubleBufferedSurface<Types...>::setBlending(bool blending)
  {
    d_buffer[0].setBlending(blending);
    d_buffer[1].setBlending(blending);
  }
}

