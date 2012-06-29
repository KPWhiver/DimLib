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

#include "DIM/shader.hpp"

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

  /* TextureBase__ */

  template<typename Type, typename ComponentType = Type>
  class TextureBase__
  {
      std::shared_ptr<GLuint> d_id;
      size_t d_height;
      size_t d_width;

      Format d_format;

      std::vector<Type> d_buffer;

      static bool s_anisotropic;
      static float s_maxAnisotropy;

    public:
      TextureBase__();

      void setBorderColor(glm::vec4 const &color) const;

      void generateMipmap() const;

      GLuint id() const;
      size_t height() const;
      size_t width() const;
      Format format() const;
      
      void update(Type *data);
      
      Type value(size_t x, size_t y, size_t channel) const;
      Type *source();

      void send(int unit, std::string const &variable) const;

      static void initialize();

    protected:
      void init(Type *data, Filtering filter, Format format, size_t width, size_t height, Wrapping wrap, int keepBuffered);
      //void updateData(Type *data) const;
      //Type *data() const;
      GLuint externalFormat() const;

    private:
      GLuint internalFormat() const;
      size_t components() const;
      GLuint depth() const;
  };

  /* Texture<Type> */

  template<typename Type = GLubyte, typename ComponentType = Type>
  class Texture: public TextureBase__<Type, ComponentType>
  {
      
      using TextureBase__<Type, ComponentType>::width;
      using TextureBase__<Type, ComponentType>::height;
      using TextureBase__<Type, ComponentType>::init;
      using TextureBase__<Type, ComponentType>::data;
      using TextureBase__<Type, ComponentType>::externalFormat;
  
  
      //std::shared_ptr<Type> d_source;
    public:

      Texture();
      Texture(Type *data, Filtering filter, Format format, size_t width, size_t height, Wrapping wrap = Wrapping::repeat, bool keepBuffered);

      //void update(Type *data);

      //Type value(size_t x, size_t y, size_t channel) const;

      //Type const *source();
  };

  /* Texture<GLubyte> */

  template<>
  class Texture<GLubyte>: public TextureBase__<GLubyte>
  {

      //using TextureBase__<GLubyte>;

      unsigned int d_source;//
      std::string d_filename;
    public:

      Texture();
      Texture(std::string const &filename, Filtering filter, Wrapping wrap = Wrapping::repeat, bool keepBuffered);
      Texture(GLubyte *data, Filtering filter, Format format, size_t width, size_t height, Wrapping wrap = Wrapping::repeat, bool keepBuffered);

      //void update(GLubyte *data);
      void reset();
      void save(std::string const &filename = "");

      //GLubyte value(size_t x, size_t y, size_t channel) const;

      //GLubyte const *source();
  };

  /* Some template meta-programming */

  template<typename Type>
  class DataType;
  //{
  //  static_assert(false, "Only GLfloat, GLuint, GLint, GLushort, GLshort, GLubyte and GLbyte are supported Texture types");
  //};

  template<>
  class DataType<GLubyte>
  {
    enum
    { value = GL_UNSIGNED_BYTE};
  };

  template<>
  class DataType<GLbyte>
  {
    enum
    { value = GL_BYTE};
  };

  template<>
  class DataType<GLushort>
  {
    enum
    { value = GL_UNSIGNED_SHORT};
  };

  template<>
  class DataType<GLshort>
  {
    enum
    { value = GL_SHORT};
  };

  template<>
  class DataType<GLuint>
  {
    enum
    { value = GL_UNSIGNED_INT};
  };

  template<>
  class DataType<GLint>
  {
    enum
    { value = GL_INT};
  };

  template<>
  class DataType<GLfloat>
  {
    enum
    { value = GL_FLOAT};
  };

  /* ****************************** */

  template<typename Type, typename ComponentType>
  Texture<Type, ComponentType>::Texture()
  {
    GLubyte data(0);
    init(&data, Filtering::nearest, Format::R8, 1, 1, Wrapping::repeat, false);
  }

  template<typename Type, typename ComponentType>
  Texture<Type, ComponentType>::Texture(Type * data, Filtering filter, Format format, size_t width, size_t height, Wrapping wrap, bool keepBuffered)
  {
    init(data, filter, format, width, height, wrap, keepBuffered);
  }
}

#include "texture.inl"
#endif
