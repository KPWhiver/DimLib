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

#include <png.h>
#include <fstream>
#include <iostream>

#include "dim/texture.hpp"

using namespace glm;
using namespace std;

//GLuint Texture::FBO()
//{
//	return d_idFBO;
//}
namespace dim
{
  namespace
  {
    void readFromStream(png_structp pngPtr, png_bytep data, png_size_t length)
    {
      png_voidp ptr = png_get_io_ptr(pngPtr);
      reinterpret_cast<std::istream*>(ptr)->read(reinterpret_cast<char*>(data), length);
    }

    void writeToStream(png_structp pngPtr, png_bytep data, png_size_t length)
    {
      png_voidp ptr = png_get_io_ptr(pngPtr);
      reinterpret_cast<std::ostream*>(ptr)->write(reinterpret_cast<char*>(data), length);
    }

    void flushStream(png_structp pngPtr)
    {
      png_voidp ptr = png_get_io_ptr(pngPtr);
      reinterpret_cast<std::ostream*>(ptr)->flush();
    }
  }

  GLubyte* Texture<GLubyte>::loadPNG(istream &input, Format &format, uint &width, uint &height)
  {
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if(!pngPtr)
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + d_filename);

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if(!infoPtr)
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + d_filename);

    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + d_filename);

    png_set_read_fn(pngPtr, reinterpret_cast<png_voidp>(&input), readFromStream);

    png_set_sig_bytes(pngPtr, 8);

    png_read_info(pngPtr, infoPtr);

    width = png_get_image_width(pngPtr, infoPtr);
    height = png_get_image_height(pngPtr, infoPtr);
    png_uint_32 color = png_get_color_type(pngPtr, infoPtr);
    png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
    png_uint_32 depth = png_get_bit_depth(pngPtr, infoPtr);

    switch (color)
    {
      case PNG_COLOR_TYPE_GRAY:
        if (depth < 8)
          png_set_expand_gray_1_2_4_to_8(pngPtr);
        depth = 8;
        format = Format::R8;
        break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
        format = Format::RG8;
        break;
      case PNG_COLOR_TYPE_PALETTE:
        png_set_palette_to_rgb(pngPtr);
        channels = 3;
      case PNG_COLOR_TYPE_RGB:
        format = Format::RGB8;
        break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
        format = Format::RGBA8;
        break;
    }

    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
    {
      png_set_tRNS_to_alpha(pngPtr);
      channels+=1;
    }

    if(depth == 16)
    {
      png_set_strip_16(pngPtr);
      depth = 8;
    }
    else if(depth < 8)
    {
      png_set_packing(pngPtr);
      depth = 8;
    }

    /* read file */
    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to load " + d_filename);

    png_bytep* rowPtrs = new png_bytep[height];

    GLubyte* data = new GLubyte[height * width * channels];

    for(uint y = 0; y != height; ++y)
    {
      png_uint_32 offset = y * (width * channels);
      rowPtrs[y] = static_cast<png_bytep>(data) + offset;
    }

    png_read_image(pngPtr, rowPtrs);

    delete[] rowPtrs;

    png_destroy_read_struct(&pngPtr, &infoPtr, 0);

    return data;
  }

  void Texture<GLubyte>::savePNG(string const &filename, ostream &output, GLubyte* data)
  {
    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(!pngPtr)
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if(!infoPtr)
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    png_set_write_fn(pngPtr, reinterpret_cast<png_voidp>(&output), writeToStream, flushStream);

    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    png_uint_32 color;
    uint channels = 0;

    switch(externalFormat())
    {
      case GL_RED:
        color = PNG_COLOR_TYPE_GRAY;
        channels = 1;
        break;
      case GL_RG:
        color = PNG_COLOR_TYPE_GRAY_ALPHA;
        channels = 2;
        break;
      case GL_RGB:
        color = PNG_COLOR_TYPE_RGB;
        channels = 3;
        break;
      case GL_RGBA:
        color = PNG_COLOR_TYPE_RGB_ALPHA;
        channels = 4;
        break;
      default:
        log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);
        break;
    }

    png_set_IHDR(pngPtr, infoPtr, width(), height(),
                 8, color, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(pngPtr, infoPtr);

    png_bytep* rowPtrs = new png_bytep[height()];

    for(uint y = 0; y != height(); ++y)
    {
      png_uint_32 offset = y * (width() * channels);
      rowPtrs[y] = static_cast<png_bytep>(data) + offset;
    }

    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    png_write_image(pngPtr, rowPtrs);

    if(setjmp(png_jmpbuf(pngPtr)))
      log(__FILE__, __LINE__, LogType::error, "Failed to save " + filename);

    png_write_end(pngPtr, 0);

    delete[] rowPtrs;
    png_destroy_write_struct(&pngPtr, &infoPtr);
  }


  Texture<GLubyte>::Texture()
  {
    init(0, Filtering::nearest, Format::R8, 0, 0, false, Wrapping::repeat);
  }

  Texture<GLubyte>::Texture(GLubyte * data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap)
  {
    init(data, filter, format, width, height, keepBuffered, wrap);
  }

  Texture<GLubyte>::Texture(string const &filename, Filtering filter, bool keepBuffered, Wrapping wrap)
      :
          d_filename(filename)
  {
    // open file
    std::ifstream file(filename.c_str());

    if(not file.is_open())
      log(__FILE__, __LINE__, LogType::error, "Failed to open " + filename + " for reading");

    // read header
    png_byte header[8];

    file.read(reinterpret_cast<char*>(header), 8);

    GLubyte *data = 0;
    Format format = Format::R8;
    uint width = 0;
    uint height = 0;

    if(not png_sig_cmp(header, 0, 8))
      data = loadPNG(file, format, width, height);
    else
      log(__FILE__, __LINE__, LogType::error, filename + " is not a valid .png file");

    init(data, filter, format, width, height, keepBuffered, wrap);

    delete[] data;
  }

  void Texture<GLubyte>::reset()
  {
    if(d_filename == "")
      return;

    // open file
    std::ifstream file(d_filename.c_str());

    if(not file.is_open())
      log(__FILE__, __LINE__, LogType::error, "Failed to open " + d_filename + " for reading");

    // read header
    char header[8];

    file.read(header, 8);

    GLubyte *data = 0;
    Format format = Format::R8;
    uint width = 0;
    uint height = 0;

    if(png_sig_cmp(reinterpret_cast<png_bytep>(header), 0, 8))
      data = loadPNG(file, format, width, height);
    else
      log(__FILE__, __LINE__, LogType::error, d_filename + " is not a valid .png file");

    update(data);

    delete[] data;
  }

  void Texture<GLubyte>::save(string filename)
  {
    if(filename == "")
      filename = d_filename;

    // open file
    std::ofstream file(filename.c_str());

    if(not file.is_open())
      log(__FILE__, __LINE__, LogType::error, "Failed to open " + filename + " for reading");

    GLubyte *data = buffer();

    savePNG(filename, file, data);
  }
}
