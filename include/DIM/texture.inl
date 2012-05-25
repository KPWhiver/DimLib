// texturebaseimplement.hpp
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
  template <typename Type, typename ComponentType>
  bool TextureBase__<Type, ComponentType>::s_anisotropic(false);

  template <typename Type, typename ComponentType>
  float TextureBase__<Type, ComponentType>::s_maxAnisotropy(0);

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::initialize()
  {
    if(GLEW_EXT_texture_filter_anisotropic)
    {
      /* It is safe to use anisotropic filtering. */
      s_anisotropic = true;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &s_maxAnisotropy);
    }
  }

  template <typename Type, typename ComponentType>
  TextureBase__<Type, ComponentType>::TextureBase__()
      :
          d_id(new GLuint(0), [](GLuint *ptr)
          { glDeleteTextures(1, ptr); delete ptr;})
  {
  }

  template <typename Type, typename ComponentType>
  GLuint TextureBase__<Type, ComponentType>::id() const
  {
    return *d_id;
  }

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::init(Type * data, Filtering filter, Format format, size_t width, size_t height, Wrapping wrap)
  {
    d_format = format;

    d_width = width;
    d_height = height;

    glGenTextures(1, d_id.get());
    glBindTexture(GL_TEXTURE_2D, *d_id);

    if(s_anisotropic == false && filter >= 0)
      filter = Filter::trilinear;

    switch(filter)
    {
      case anisotropicMax:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, s_maxAnisotropy);
        break;
      case anisotropic1x:
      case anisotropic2x:
      case anisotropic4x:
      case anisotropic8x:
      case anisotropic16x:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, filter);
      case trilinear:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      case bilinear:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      case linear:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
      case nearest:
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), d_width, d_height, 0, externalFormat(), DataType<ComponentType>::value, data);

    if(filter != linear && filter != nearest)
      glGenerateMipmap (GL_TEXTURE_2D);
  }

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::updateData(Type *data) const
  {
    glBindTexture(GL_TEXTURE_2D, *d_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, d_width, d_height, externalFormat(), DataType<ComponentType>::value, data);
  }

  template <typename Type, typename ComponentType>
  Type *TextureBase__<Type, ComponentType>::data() const
  {
    Type *texData = new Type[d_width * d_height * components()];
    glBindTexture(GL_TEXTURE_2D, *d_id);
    glGetTexImage(GL_TEXTURE_2D, 0, externalFormat(), DataType<ComponentType>::value, data);
    return texData;
  }

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::setBorderColor(vec4 const &color) const
  {
    glBindTexture(GL_TEXTURE_2D, *d_id);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
  }

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::generateMipmap() const
  {
    glBindTexture(GL_TEXTURE_2D, *d_id);
    glgenerateMipmap (GL_TEXTURE_2D);
  }

  template <typename Type, typename ComponentType>
  size_t TextureBase__<Type, ComponentType>::height() const
  {
    return d_height;
  }

  template <typename Type, typename ComponentType>
  size_t TextureBase__<Type, ComponentType>::width() const
  {
    return d_width;
  }
  
  template <typename Type, typename ComponentType>
  Format TextureBase__<Type, ComponentType>::format() const
  {
    return d_format;
  }

  template <typename Type, typename ComponentType>
  TextureBase__<Type, ComponentType>::components()
  {
    switch(externalFormat())
    {
      case GL_RGBA:
        return 4;
      case GL_RGB:
        return 3;
      case GL_RG:
        return 2;
      case GL_R:
        return 1;
      case GL_DEPTH_COMPONENT:
        return 1;
      default:
        throw(runtime_error("Unknown Texture format used in TextureBase__::components()"));
    }
    return 0;
  }

  template <typename Type, typename ComponentType>
  GLuint TextureBase__<Type, ComponentType>::externalFormat() const
  {
    switch(d_format)
    {
      case Format::RGBA8:
      case Format::sRGB8A8:
      case Format::RGBA16:
      case Format::RGBA32:
        return GL_RGBA;
      case Format::RGB8:
      case Format::sRGB8:
      case Format::RGB16:
      case Format::RGB32:
      case Format::R11G11B10:
        return GL_RGB;
      case Format::RG8:
      case Format::RG16:
      case Format::RG32:
        return GL_RG;
      case Format::R8:
      case Format::R16:
      case Format::R32:
        return GL_R;
      case Format::D16:
      case Format::D32:
        return GL_DEPTH_COMPONENT;
      default:
        throw(runtime_error("Unknown Texture format used in TextureBase__::externalFormat()"));
    }
    return 0;
  }

  template <typename Type, typename ComponentType>
  GLuint TextureBase__<Type, ComponentType>::internalFormat() const
  {
    GLuint dataType = DataType<ComponentType>::value;

    switch(format)
    {
      /* 8-bit formats */
      case RGBA8:
        switch(dataType)
        {
          case GL_INT:
            return GL_RGBA8I;
          case GL_UNSIGNED_INT:
            return GL_RGBA8UI;
        }
        return GL_RGBA8;
      case RGB8:
        switch(dataType)
        {
          case GL_INT:
            return GL_RGB8I;
          case GL_UNSIGNED_INT:
            return GL_RGB8UI;
        }
        return GL_RGB8;
      case RG8:
        d_internalFormat = GL_RG8;
        switch(dataType)
        {
          case GL_INT:
            return GL_RG8I;
          case GL_UNSIGNED_INT:
            return GL_RG8UI;
        }
        return GL_RG8;
      case R8:
        switch(dataType)
        {
          case GL_INT:
            return GL_R8I;
          case GL_UNSIGNED_INT:
            return GL_R8UI;
        }
        return GL_R8;
      case sRGB8A8:
        return GL_SRGB8_ALPHA8;
      case sRGB8:
        return GL_SRGB8;

        /* 16-bit formats */
      case RGBA16:
        switch(dataType)
        {
          case GL_FLOAT:
            return GL_RGBA16F;
          case GL_INT:
            return GL_RGBA16I;
          case GL_UNSIGNED_INT:
            return GL_RGBA16UI;
        }
        return GL_RGBA16;
      case RGB16:
        switch(dataType)
        {
          case GL_FLOAT:
            return GL_RGB16F;
          case GL_INT:
            return GL_RGB16I;
          case GL_UNSIGNED_INT:
            return GL_RGB16UI;
        }
        return GL_RGB16;
      case RG16:
        switch(dataType)
        {
          case GL_FLOAT:
            d_internalFormat = GL_RG16F;
          case GL_INT:
            return GL_RG16I;
          case GL_UNSIGNED_INT:
            return GL_RG16UI;
        }
        return GL_RG16;
      case R16:
        switch(dataType)
        {
          case GL_FLOAT:
            return GL_R16F;
          case GL_INT:
            return GL_R16I;
          case GL_UNSIGNED_INT:
            return GL_R16UI;
        }
        return GL_R16;
      case D16:
        return GL_DEPTH_COMPONENT16;

        /* 32-bit formats */
      case RGBA32:
        switch(dataType)
        {
          case GL_INT:
            return GL_RGBA32I;
          case GL_UNSIGNED_INT:
            return GL_RGBA32UI;
        }
        return GL_RGBA32F;
      case RGB32:
        switch(dataType)
        {
          case GL_INT:
            return GL_RGB32I;
          case GL_UNSIGNED_INT:
            return GL_RGB32UI;
        }
        return GL_RGB32F;
      case RG32:
        switch(dataType)
        {
          case GL_INT:
            return GL_RG32I;
          case GL_UNSIGNED_INT:
            return GL_RG32UI;
        }
        return GL_RG32F;
      case R32:
        switch(dataType)
        {
          case GL_INT:
            return GL_R32I;
          case GL_UNSIGNED_INT:
            return GL_R32UI;
        }
        return GL_R32F;
      case D32:
        return GL_DEPTH_COMPONENT32;

      case R11G11B10:
        return GL_R11F_G11F_B10F;

      default:
        throw(runtime_error("Unknown Texture format used in TextureBase__::internalFormat()"));
    }
    return 0;
  }

  template <typename Type, typename ComponentType>
  void TextureBase__<Type, ComponentType>::send(int unit, string const &variable) const
  {
    int textureUnit = GL_TEXTURE0 + unit;

    glActiveTexture(textureUnit);
    Shader::active().send(unit, variable);
    glBindTexture(GL_TEXTURE_2D, *d_id);
  }
}
