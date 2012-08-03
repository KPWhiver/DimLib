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
  {
    GLubyte data(0);
    init(&data, Filtering::nearest, Format::R8, 1, 1, false, Wrapping::repeat);
  }

  Texture<GLubyte>::Texture(GLubyte * data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }

  Texture<GLubyte>::Texture(string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap)
  {
    ILuint source;

    ilGenImages(1, &source);
    ilBindImage (source);

    if(ilLoadImage(filename.c_str()) == false)
    {

      ilDeleteImages(1, &source);
      throw(runtime_error("Unable to load: " + filename + ", unknown file format"));
    }

    int format = ilGetInteger(IL_IMAGE_FORMAT);
    uint pixelDepth = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

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

    init(ilGetData(), filter, form, ilGetInteger(IL_IMAGE_WIDTH),
                            ilGetInteger(IL_IMAGE_HEIGHT), keepBuffered, wrap);

    d_filename = filename;

    ilDeleteImages(1, &source);
  }

  void Texture<GLubyte>::reset()
  {
    ILuint source;

    if(d_filename == "")
      return;

    ilGenImages(1, &source);

    ilBindImage(source);
    ilLoadImage(d_filename.c_str());

    int format = ilGetInteger(IL_IMAGE_FORMAT);
    if(format == IL_BGR)
      ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

    if(format == IL_BGRA)
      ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    update(ilGetData());

    ilDeleteImages(1, &source);
  }

  void Texture<GLubyte>::save(string filename)
  {
    ILuint source;

    if(filename == "")
      filename = d_filename;

    uint bbp = 0;
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
      default:
        std::stringstream ss;
        ss << "Unable to save texture with format: " << externalFormat();
        throw(std::runtime_error(ss.str()));
    }

    GLubyte *dataSource = buffer();

    ilGenImages(1, &source);
    ilBindImage(source);
    ilTexImage(width(), height(), 1, bbp, format, IL_UNSIGNED_BYTE, dataSource);
    ilSaveImage(filename.c_str());
    ilDeleteImages(1, &source);
  }
}
