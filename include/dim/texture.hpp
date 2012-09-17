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

#include "dim/dim.hpp"

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

  enum class Format
  {
    RGBA8,
    RGBA16,
    RGBA32,
    sRGB8A8,
    RGB8,
    RGB16,
    RGB32,
    sRGB8,
    RG8,
    RG16,
    RG32,
    R8,
    R16,
    R32,
    D16,
    D32,
    R11G11B10,
  };

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

      Format d_format;

      /* local texture buffer */
      std::vector<Type> d_buffer;
      bool d_keepBuffered;
      bool d_outdatedBuffer;
      uint d_bufferLevel;

    public:
      TextureBase();

      /* texture properties */
      void setBorderColor(glm::vec4 const &color) const;

      void generateMipmap() const;

      GLuint id() const;
      uint height() const;
      uint width() const;
      Format format() const;
      
      void bind() const;
      
      /* local texture buffer */
      void renewBuffer();
      void update(Type *data = 0);
      
      Type value(uint x, uint y, uint channel, uint level = 0);
      Type *buffer(uint level = 0);

    protected:
      void init(Type *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap);
      GLuint externalFormat() const;

    private:
      GLuint internalFormat() const;
      uint components() const;
      GLuint depth() const;
  };

  }
  /* Texture<Type> */

  template<typename Type = GLubyte>
  class Texture: public internal::TextureBase<Type>
  {
      using internal::TextureBase<Type>::init;
      using internal::TextureBase<Type>::externalFormat;
  
    public:
      using internal::TextureBase<Type>::width;
      using internal::TextureBase<Type>::height;


      Texture();
      Texture(Type *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
  };

  /* Texture<GLubyte> */

  template<>
  class Texture<GLubyte>: public internal::TextureBase<GLubyte>
  {
      using internal::TextureBase<GLubyte>::externalFormat;
      //unsigned int d_source;//
      std::string d_filename;
    public:
      using internal::TextureBase<GLubyte>::width;
      using internal::TextureBase<GLubyte>::height;

      Texture();
      Texture(std::string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap = Wrapping::repeat);
      Texture(GLubyte *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap = Wrapping::repeat);

      void reset();
      void save(std::string filename = "");

    private:
      GLubyte* loadPNG(std::istream &input, Format &format, uint &width, uint &height);
      void savePNG(std::string const &filename, std::ostream &output, GLubyte* data);
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
    Type data(0);
    init(&data, Filtering::nearest, Format::R8, 1, 1, false, Wrapping::repeat);
  }

  template<typename Type>
  Texture<Type>::Texture(Type * data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }
}

#include "texture.inl"
#endif
