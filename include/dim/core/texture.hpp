//      textures.hpp
//      
//      Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <string>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "dim/core/dim.hpp"

namespace dim
{
  enum class Filtering : int
  {
    nearest = -1,
    linear = -2,
    bilinear = -3,
    trilinear = -4,
    anisotropic1x = 1,
    anisotropic2x = 2,
    anisotropic4x = 4,
    anisotropic8x = 8,
    anisotropic16x = 16,
    anisotropicMax = 0,
  };

  enum class Wrapping
  {
    edgeClamp = GL_CLAMP_TO_EDGE,
    borderClamp = GL_CLAMP_TO_BORDER,
    repeat = GL_REPEAT,
    mirroredRepeat = GL_MIRRORED_REPEAT,
  };

  enum class NormalizedFormat
  {
    RGBA8 = GL_RGBA8,
    RGBA16 = GL_RGBA16,
    sRGB8A8 = GL_SRGB8_ALPHA8,
    RGB8 = GL_RGB8,
    RGB16 = GL_RGB16,
    sRGB8 = GL_SRGB8,
    RG8 = GL_RG8,
    RG16 = GL_RG16,
    R8 = GL_R8,
    R16 = GL_R16,
    D16 = GL_DEPTH_COMPONENT16,
    D32 = GL_DEPTH_COMPONENT32,
  };

  enum class Format
  {
    RGBA8,
    RGBA16,
    RGBA32,
    RGB8,
    RGB16,
    RGB32,
    RG8,
    RG16,
    RG32,
    R8,
    R16,
    R32,
    D32,
    R11G11B10,
  };

  /*enum class Normalization
  {
    none,
    sign,
    unsign,
  };*/

  namespace internal
  {
  /* internal::TextureBase */
  class TextureProperties
  {
    protected:
      /* static texture properties */
      static bool s_anisotropic;
      static float s_maxAnisotropy;
      static float s_maxTextureSize;

      static bool s_integerFormats;
      static bool s_floatFormats;

      static bool s_initialized;
      static void initialize();
  };


  template<typename Type>
  class TextureBase : public TextureProperties
  {
      /* texture properties */
      std::shared_ptr<GLuint> d_id;
      uint d_height;
      uint d_width;

      GLuint d_internalFormat;
      Filtering d_filter;
      Wrapping d_wrapping;
      
      glm::vec4 d_borderColor;

      bool d_keepBuffered;
      /* local texture buffer */
      mutable std::vector<Type> d_buffer;
      mutable bool d_outdatedBuffer;
      mutable uint d_bufferLevel;

    public:
      TextureBase();

      /* texture properties */
      void setBorderColor(glm::vec4 const &color);
      void setWrapping(Wrapping wrap);

      void generateMipmap();

      GLuint id() const;
      uint height() const;
      uint width() const;

      Filtering filter() const;
      Wrapping wrapping() const;

      glm::vec4 const &borderColor() const;
      bool buffered() const;
      
      void bind() const;
      
      /* local texture buffer */
      void renewBuffer();
      void update(Type const *data = 0);
      
      Type value(uint x, uint y, uint channel, uint level = 0) const;
      std::vector<Type> const &buffer(uint level = 0) const;
      std::vector<Type> &buffer(uint level = 0);

    protected:
      void init(Type const *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap);
      void init(Type const *data, Filtering filter, NormalizedFormat format, uint width, uint height, bool keepBuffered, Wrapping wrap);
      void init(Type const *data, Filtering filter, GLuint format, uint width, uint height, bool keepBuffered, Wrapping wrap);
      GLuint externalFormat() const;
      GLuint internalFormat() const;

    private:
      uint components() const;
      GLuint depth() const;
  };

  }
  /* Texture<Type> */

  template<typename Type = GLubyte>
  class Texture: public internal::TextureBase<Type>
  {
    template<typename ...Types>
    friend class Surface;

      using internal::TextureBase<Type>::init;
      using internal::TextureBase<Type>::externalFormat;
      using internal::TextureBase<Type>::internalFormat;

    public:
      using internal::TextureBase<Type>::width;
      using internal::TextureBase<Type>::height;
      using internal::TextureBase<Type>::buffer;
      using internal::TextureBase<Type>::borderColor;
      using internal::TextureBase<Type>::filter;
      using internal::TextureBase<Type>::wrapping;
      using internal::TextureBase<Type>::buffered;

      Texture();
      Texture(Type const *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
      Texture(Type const *data, Filtering filter, NormalizedFormat format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);

      Texture<Type> copy() const;

    private:
      Texture(Type const *data, Filtering filter, GLuint format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
  };

  /* Texture<GLubyte> */

  template<>
  class Texture<GLubyte>: public internal::TextureBase<GLubyte>
  {
      template<typename ...Types>
      friend class Surface;

      using internal::TextureBase<GLubyte>::externalFormat;
      using internal::TextureBase<GLubyte>::internalFormat;

      std::string d_filename;
    public:
      using internal::TextureBase<GLubyte>::width;
      using internal::TextureBase<GLubyte>::height;
      using internal::TextureBase<GLubyte>::buffer;
      using internal::TextureBase<GLubyte>::borderColor;
      using internal::TextureBase<GLubyte>::filter;
      using internal::TextureBase<GLubyte>::wrapping;
      using internal::TextureBase<GLubyte>::buffered;

      static Texture<GLubyte> const &zeroTexture();

      Texture();
      Texture(std::string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
      Texture(GLubyte const *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
      Texture(GLubyte const *data, Filtering filter, NormalizedFormat format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);

      Texture<GLubyte> copy() const;

      void reset();
      void save(std::string filename = "") const;

    private:
      std::vector<GLubyte> loadPNG(std::istream &input, NormalizedFormat &format, uint &width, uint &height);
      void savePNG(std::string const &filename, std::ostream &output, std::vector<GLubyte> const &data) const;

      Texture(GLubyte const *data, Filtering filter, GLuint format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
  };

  /* Some template meta-programming */
  namespace internal
  {

    template<typename Type>
    struct DataType;

    template<>
    struct DataType<GLubyte>
    {
      enum
      { value = GL_UNSIGNED_BYTE};
    };

    template<>
    struct DataType<GLbyte>
    {
      enum
      { value = GL_BYTE};
    };

    template<>
    struct DataType<GLushort>
    {
      enum
      { value = GL_UNSIGNED_SHORT};
    };

    template<>
    struct DataType<GLshort>
    {
      enum
      { value = GL_SHORT};
    };

    template<>
    struct DataType<GLuint>
    {
      enum
      { value = GL_UNSIGNED_INT};
    };

    template<>
    struct DataType<GLint>
    {
      enum
      { value = GL_INT};
    };

    template<>
    struct DataType<GLfloat>
    {
      enum
      { value = GL_FLOAT};
    };
  }
  /* ****************************** */

  template<typename Type>
  Texture<Type>::Texture()
  {
    init(0, Filtering::nearest, NormalizedFormat::R8, 0, 0, false, Wrapping::repeat);
  }

  template<typename Type>
  Texture<Type>::Texture(Type const *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }
  
  template<typename Type>
  Texture<Type>::Texture(Type const *data, Filtering filter, NormalizedFormat format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }

  template<typename Type>
  Texture<Type>::Texture(Type const *data, Filtering filter, GLuint format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }

  template<typename Type>
  Texture<Type> Texture<Type>::copy() const
  {
    Texture<Type> texture(buffer().data(), filter(), internalFormat(), width(), height(), buffered(), wrapping());
    
    if(borderColor() != glm::vec4(0))
      texture.setBorderColor(borderColor());

    return texture;
  }
}

#include "texture.inl"
#endif
