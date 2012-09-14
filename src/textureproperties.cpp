// textureproperties__.cpp
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

#include "dim/texture.hpp"

namespace dim
{
  namespace internal
  {
  /* static texture properties */
  bool TextureProperties::s_anisotropic(false);

  float TextureProperties::s_maxAnisotropy(0);

  float TextureProperties::s_maxTextureSize(0);

  bool TextureProperties::s_integerFormats(false);

  bool TextureProperties::s_floatFormats(false);

  bool TextureProperties::s_initialized(false);

  void TextureProperties::initialize()
  {
    s_initialized = true;

    if(GLEW_EXT_texture_filter_anisotropic)
    {
      // it is safe to use anisotropic filtering
      s_anisotropic = true;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &s_maxAnisotropy);
    }

    glGetFloatv(GL_MAX_TEXTURE_SIZE, &s_maxTextureSize);

    s_integerFormats = (GLEW_EXT_texture_integer ? true : false);
    s_floatFormats = (GLEW_ARB_texture_float ? true : false);
  }
  }
}
