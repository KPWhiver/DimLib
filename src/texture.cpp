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

#include <IL/il.h>

#include "DIM/texture.hpp"
#include "DIM/shader.hpp"

using namespace glm;
using namespace std;

//GLuint Texture::FBO()
//{
//	return d_idFBO;
//}
namespace dim
{
  Texture<GLubyte>::Texture()
      :
          d_source(0)
  {
    GLubyte data(0);
    init(&data, Texture::nearest, Format::R8, 1, 1, Wrapping::repeat);
  }

  Texture<GLubyte>::Texture(GLubyte * data, Filtering filter, Format format, size_t width, size_t height, Wrap wrap)
      :
          d_source(0)
  {
    init(data, filter, format, width, height, wrap);
  }

  Texture<GLubyte>::Texture(string const &filename, Filtering filter, bool edit, Wrap wrap)
  {
    ilGenImages(1, &d_source);
    ilBindImage (d_source);

    if(ilLoadImage(filename.c_str()) == false)
    {

      ilDeleteImages(1, &d_source);
      throw(runtime_error("Unable to load: " + filename + ", unknown file format"));
    }

    int format = ilGetInteger(IL_IMAGE_FORMAT);
    size_t pixelDepth = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    d_width = ilGetInteger(IL_IMAGE_WIDTH);
    d_height = ilGetInteger(IL_IMAGE_HEIGHT);

    if(format == IL_BGR)
      ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

    if(format == IL_BGRA)
      ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    Format form;

    switch(pixelDepth)
    {
      case 1:
        form = Format::R8;
        break;
      case 3:
        form = Format::RGB8;
        break;
      case 4:
        form = Format::RGBA8;
        break;
      default:
        throw runtime_error("Unable to load: " + filename + ", depth is not loadable");
    }

    init(static_cast<void*>(ilGetData()), filter, form, d_width, d_height, wrap);

    d_filename = filename;

    if(edit == false)
    {
      ilDeleteImages(1, &d_source);
      d_source = 0;
    }
  }

  void Texture<GLubyte>::reset()
  {
    if(d_filename == "")
      return;

    bool edit = true;

    if(d_source == 0)
    {
      ilGenImages(1, &d_source);
      edit = false;
    }

    ilBindImage(d_source);
    ilLoadImage(d_filename.c_str());

    int format = ilGetInteger(IL_IMAGE_FORMAT);
    if(format == IL_BGR)
      ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

    if(format == IL_BGRA)
      ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    updateData(ilGetData());

    if(edit == false)
    {
      ilDeleteImages(1, &d_source);
      d_source = 0;
    }
  }

  void Texture<GLubyte>::update(GLubyte* data)
  {
    updateData(data);

    if(d_source != 0)
    {
      size_t pixelDepth = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
      int format = ilGetInteger(IL_IMAGE_FORMAT);

      ilTexImage(d_width, d_height, 1, pixelDepth, format, IL_UNSIGNED_BYTE, data);
    }
  }

  void Texture<GLubyte>::save(string const &filename)
  {
    if(filename == "")
      filename = d_filename;

    if(d_source != 0)
    {
      ilBindImage(d_source);
      ilSaveImage(filename.c_str());
    }
    else
    {
      size_t bbp = 0;
      int format = IL_ALPHA;
      switch(externalFormat())
      {
        case GL_R:
          bbp = 1;
          break;
        case GL_RGB:
          bbp = 3;
          format = IL_RGB;
          break;
        case GL_RGBA:
          bbp = 4;
          format = IL_RGBA;
          break;
      }

      Type *dataSource = source();

      ilGenImages(1, &d_source);
      ilBindImage(d_source);
      ilTexImage(d_width, d_height, 1, bbp, format, IL_UNSIGNED_BYTE, dataSource);
      ilSaveImage(filename.c_str());
      ilDeleteImages(1, &d_source);
      d_source = 0;
      delete[] dataSource;

    }
  }

  GLubyte Texture<GLubyte>::value(size_t x, size_t y, size_t channel) const
  {
    GLubyte *dataSource;

    if(d_source != 0)
    {
      ilBindImage(d_source);
      dataSource = ilGetData();
    }
    else
    {
      dataSource = source();
    }

    switch(externalFormat())
    {
      case GL_R:
        if(channel == 0)
          return dataSource[(y * width() + x)];
        break;
      case GL_RGB:
        if(channel < 3)
          return dataSource[(y * width() + x) * 3 + channel];
        break;
      case GL_RGBA:
        if(channel < 4)
          return dataSource[(y * width() + x) * 4 + channel];
        break;
    }

    if(d_source == 0)
      delete[] dataSource;

    return 0;
  }

  GLubyte *Texture<GLubyte>::source()
  {
    ilBindImage(d_source);
    return ilGetData();
  }
}
