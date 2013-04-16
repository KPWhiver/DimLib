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

#include "dim/core/texture.hpp"
#include "dim/core/shader.hpp"
#include "dim/core/tools.hpp"

namespace dim
{
  class Window;

  namespace internal
  {
    class SurfaceBase
    {
      friend class dim::Window;

      protected:
        static SurfaceBase* s_renderTarget;

        static GLint s_maxAttachment;

        SurfaceBase();
        virtual ~SurfaceBase();

      public:
        virtual void finishRendering();
    };

    void setViewport(int x, int y, int width, int height);
    void setBlending(bool blend);
  }

  template<typename ...Types>
  class Surface : public internal::SurfaceBase
  {
    template<typename ...DDTypes>
    friend class DoubleBufferedSurface;

    typedef std::tuple<Texture<Types>...> TupleType;
    typedef std::tuple<Texture<Types>*...> TuplePtrType;
    
    static Shader s_copy;

    TupleType d_textures;
    TuplePtrType d_targets;

    std::shared_ptr<GLuint> d_id;

    GLuint d_attachments[std::tuple_size<TupleType>::value];

    uint d_colorAttachments;

    bool d_depthComponent;
    bool d_colorComponent[4];
    
    uint d_minWidth;
    uint d_minHeight;
    
    glm::vec4 d_clearColor;
    float d_clearDepth;

    bool d_blending;

    enum ComponentType
    {
      depth, 
      color, 
      stencil,
    };
    
  public:
    Surface(uint width, uint height, Format format, Filtering filter = Filtering::linear);
    Surface(uint width, uint height, NormalizedFormat format, Filtering filter = Filtering::linear);
    explicit Surface(typename std::tuple_element<0, TuplePtrType>::type ptr);

    Surface(Surface const &other) = delete;
    Surface(Surface &&tmp) = default;

    Surface& operator=(Surface const &other) = delete;
    Surface& operator=(Surface &&tmp) = default;
    
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

    void setBlending(bool blending);

    void clear();

    void renderTo(bool clearBuffer = true);
    void renderToPart(uint x, uint y, uint width, uint height, bool clearBuffer);

    template<typename Type, uint Index = 0>
    void copy(Texture<Type> const &source);
    
    template<typename Type, uint Index = 0>
    void copyToPart(Texture<Type> const &source, uint x, uint y, uint width, uint height);

    GLuint id() const;

  private:
    explicit Surface(typename std::tuple_element<0, TupleType>::type obj);

    template<uint Index>
    void addTarget(typename std::tuple_element<Index, TupleType>::type obj);

	  Surface::ComponentType processFormat(GLuint format);

	  void notifyTextures();

    template<uint Index>
    void addAttachment(ComponentType attachment, typename std::tuple_element<Index, TuplePtrType>::type ptr);
  };

  namespace internal
  {
    //TupleCaller class
    template<uint Index, typename TupleCallType>
    class TupleCaller
    {
        TupleCaller<Index - 1, TupleCallType> d_next;

      public:
        TupleCaller(TupleCallType &tuple)
        :
          d_next(tuple)
        {
          std::get<Index>(tuple)->renewBuffer();
        }
    };

    template<typename TupleCallType>
    class TupleCaller<0, TupleCallType>
    {
      public:
        TupleCaller(TupleCallType &tuple)
        {
          std::get<0>(tuple)->renewBuffer();
        }
    };
    //--
  }

  template<>
  class Surface<>
  {
      std::shared_ptr<GLuint> d_id;
    
    public:
      Surface(uint width, uint height);
  };
}

#include "surface.inl"

#endif
