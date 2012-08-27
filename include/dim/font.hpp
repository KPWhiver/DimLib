// font.hpp
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

#ifndef FONT_HPP
#define FONT_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <string>
#include <memory>

#include "dim/texture.hpp"

namespace dim
{

  class Font
  {
    FT_Face d_face;

    FT_Glyph d_glyphs[128];
    static FT_Library s_library;

  public:
    Font(std::string filename);

    static void initialize();

    Texture<GLubyte> generateTexture(std::string const &text, size_t width, size_t height) const;
    //GLuint letter(size_t ch);

  private:
    size_t nextPowerOf2(size_t number);
    void generateCharMap(size_t ch);

  };;


}
#endif
