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


#ifndef TEXTURES
#define TEXTURES

#include <string>
#include <memory>

#include "DIM/dim.hpp"

namespace dim
{

class Texture
{
	std::shared_ptr<GLuint> d_id;

	unsigned int d_source;
  std::string d_filename;
  size_t d_depth;
  size_t d_height;
  size_t d_width;
  
  GLuint d_externalFormat;
  GLuint d_internalFormat;
  GLuint d_dataType;

  static bool s_anisotropic;
  static float s_maxAnisotropy;
    
public:
  enum Filtering
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

  enum Wrap
  {
    edgeClamp = GL_CLAMP_TO_EDGE,
    borderClamp = GL_CLAMP_TO_BORDER,
    repeat = GL_REPEAT,
    mirroredRepeat = GL_MIRRORED_REPEAT,
  };

  enum Format
  {
    rgba8,
    rgba16,
    rgba32,
    rgb8,
    rgb16,
    rgb32,
    rg8,
    rg16,
    rg32,
    r8,
    r16,
    r32,
    depth16,
    depth32,
    //r11g11b10,
  };

  Texture();
  Texture(std::string const &filename, Filtering filter, bool edit, Wrap wrap = repeat);
  Texture(void * data, Filtering filter, Format format, size_t width, size_t height, Wrap wrap = repeat);

  void setBorderColor(glm::vec4 const &color);

  void update(void* data);
  void reset();
  void save();
  
  size_t height() const;
  size_t width() const;
  
  GLubyte value(size_t x, size_t y, size_t channel) const;

  void send(int unit, std::string const &variable) const;

  GLubyte *source();
  GLuint id() const;

  static void initialize();

private:

  void processFormat(Format format);

  void init(void * data, Filtering filter, Wrap wrap);

};

}
#endif
