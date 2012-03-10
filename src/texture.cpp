// texture.cpp
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
#include <cctype>
#include <stdexcept>

#include <IL/il.h>

#include "GL/glm.hpp"
//#include "main_objects.hpp"
#include "DIM/texture.hpp"
#include "DIM/window.hpp"
#include "DIM/shader.hpp"

using namespace glm;
using namespace std;

//GLuint Texture::FBO()
//{
//	return d_idFBO;
//}
namespace dim
{
  bool Texture::s_anisotropic(false);
  float Texture::s_maxAnisotropy(0);

  void Texture::initialize()
  {
    if(GLEW_EXT_texture_filter_anisotropic)
    {
      /* It is safe to use anisotropic filtering. */
      s_anisotropic = true;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &s_maxAnisotropy);
    }
  }

  GLubyte *Texture::source()
  {
    ilBindImage(d_source);
    return ilGetData();
  }

  GLuint Texture::id() const
  {
    return *d_id;
  }

  Texture::Texture()
      : d_id(new GLuint(0), [](GLuint *ptr)
      { glDeleteTextures(1, ptr); delete ptr;}), d_depth(8), d_height(1), d_width(1), d_externalFormat(
          GL_RED), d_internalFormat(GL_R8), d_dataType(GL_UNSIGNED_BYTE)
  {
    processFormat(r8);
    GLubyte data(0);
    init(&data, Texture::nearest);
  }

  Texture::Texture(void * data, Filtering filter, Format format, size_t width, size_t height)
      : d_id(new GLuint, [](GLuint *ptr)
      { glDeleteTextures(1, ptr); delete ptr;}), d_depth(8), d_height(height), d_width(width), d_externalFormat(
          GL_RED), d_internalFormat(GL_R8), d_dataType(GL_UNSIGNED_BYTE)
  {
    processFormat(format);
    init(data, filter);
  }

  void Texture::init(void * data, Filtering filter)
  {
    glGenTextures(1, d_id.get());
    glBindTexture(GL_TEXTURE_2D, *d_id);

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

    if(d_depth != 8 && d_externalFormat != GL_DEPTH_COMPONENT)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    GLint GenMipMap;

    if(filter != linear && filter != nearest)
    {
      glGetTexParameteriv(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, &GenMipMap);
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, d_internalFormat, d_width, d_height, 0, d_externalFormat,
        d_dataType, data);
    /*
     * The following is the openGL 3 compliant code:
     *  if(mipmap)
     *  {
     *    glGenerateMipmap(GL_TEXTURE_2D);
     *  }
     */

    if(filter != linear && filter != nearest)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GenMipMap);
    }
  }

  Texture::Texture(string const &filename, Filtering filter, bool edit)
      : d_id(new GLuint, [](GLuint *ptr)
      { delete ptr;}), d_depth(8), d_externalFormat(GL_ALPHA), d_internalFormat(GL_ALPHA), d_dataType(
          GL_UNSIGNED_BYTE)
  {
    ilGenImages(1, &d_source);
    ilBindImage(d_source);

    if(ilLoadImage(filename.c_str()) == false)
    {
      //TODO throw
      ilDeleteImages(1, &d_source);
      return;
    }

    int format = ilGetInteger(IL_IMAGE_FORMAT);
    size_t pixelDepth = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    d_width = ilGetInteger(IL_IMAGE_WIDTH);
    d_height = ilGetInteger(IL_IMAGE_HEIGHT);

    if(format == IL_BGR)
      ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

    if(format == IL_BGRA)
      ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    switch(pixelDepth)
    {
      case 1:
        d_internalFormat = GL_ALPHA;
        d_externalFormat = GL_ALPHA;
        break;
      case 3:
        d_internalFormat = GL_RGB;
        d_externalFormat = GL_RGB;
        break;
      case 4:
        d_internalFormat = GL_RGBA;
        d_externalFormat = GL_RGBA;
        break;
      default:
        throw runtime_error("Unsupported Texture format passed");
    }

    init(static_cast<void*>(ilGetData()), filter);

    d_filename = filename;

    if(edit == false)
    {
      ilDeleteImages(1, &d_source);
      d_source = 0;
    }
  }

  GLubyte Texture::value(size_t x, size_t y, size_t channel) const
  {
    if(d_source == 0)
      return 0;

    ilBindImage(d_source);

    switch(d_externalFormat)
    {
      case GL_ALPHA:
        if(channel == 0)
          return ilGetData()[(y * d_width + x)];
        break;
      case GL_RGB:
        if(channel < 3)
          return ilGetData()[(y * d_width + x) * 3 + channel];
        break;
      case GL_RGBA:
        if(channel < 4)
          return ilGetData()[(y * d_width + x) * 4 + channel];
        break;
    }

    return 0;
  }

  size_t Texture::height() const
  {
    return d_height;
  }

  size_t Texture::width() const
  {
    return d_width;
  }

  void Texture::reset()
  {
    ilBindImage(d_source);
    ilLoadImage(d_filename.c_str());
    update(ilGetData());
  }

  void Texture::update(void* data)
  {
    if(d_width == 0)
      return;

    glBindTexture(GL_TEXTURE_2D, *d_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, d_width, d_height, d_externalFormat, d_dataType, data);
  }

  void Texture::save()
  {
    ilBindImage(d_source);
    ilSaveImage(d_filename.c_str());
  }

  void Texture::processFormat(Format format)
  {
    switch(format)
    {
      case rgba8:
        d_internalFormat = GL_RGBA8;
        d_externalFormat = GL_RGBA;
        break;
      case rgb8:
        d_internalFormat = GL_RGB8;
        d_externalFormat = GL_RGB;
        break;
      case rg8:
        d_internalFormat = GL_RGBA8;
        d_externalFormat = GL_RG;
        break;
      case r8:
        d_internalFormat = GL_R8;
        d_externalFormat = GL_RED;
        break;

      case rgba16:
        d_internalFormat = GL_RGBA16F;
        d_externalFormat = GL_RGBA;
        d_dataType = GL_HALF_FLOAT;
        d_depth = 16;
        break;
      case rgb16:
        d_internalFormat = GL_RGB16F;
        d_externalFormat = GL_RGB;
        d_dataType = GL_HALF_FLOAT;
        d_depth = 16;
        break;
      case rg16:
        d_internalFormat = GL_RG16F;
        d_externalFormat = GL_RG;
        d_dataType = GL_HALF_FLOAT;
        d_depth = 16;
        break;
      case r16:
        d_internalFormat = GL_R16F;
        d_externalFormat = GL_RED;
        d_dataType = GL_HALF_FLOAT;
        d_depth = 16;
        break;
      case depth16:
        d_internalFormat = GL_DEPTH_COMPONENT16;
        d_externalFormat = GL_DEPTH_COMPONENT;
        d_depth = 16;
        break;

      case rgba32:
        d_internalFormat = GL_RGBA32F;
        d_externalFormat = GL_RGBA;
        d_dataType = GL_FLOAT;
        d_depth = 32;
        break;
      case rgb32:
        d_internalFormat = GL_RGB32F;
        d_externalFormat = GL_RGB;
        d_dataType = GL_FLOAT;
        d_depth = 32;
        break;
      case rg32:
        d_internalFormat = GL_RG32F;
        d_externalFormat = GL_RG;
        d_dataType = GL_HALF_FLOAT;
        d_depth = 32;
        break;
      case r32:
        d_internalFormat = GL_R32F;
        d_externalFormat = GL_RED;
        d_dataType = GL_FLOAT;
        d_depth = 32;
        break;
      case depth32:
        d_internalFormat = GL_DEPTH_COMPONENT32;
        d_externalFormat = GL_DEPTH_COMPONENT;
        d_depth = 32;
        break;
    }
  }

  void Texture::send(int unit, string const &variable) const
  {
    if(d_width == 0)
      return;

    int textureUnit;
    textureUnit = GL_TEXTURE0 + unit;

    glActiveTexture(textureUnit);
    Shader::active().send(unit, variable);
    glBindTexture(GL_TEXTURE_2D, *d_id);
  }
}
