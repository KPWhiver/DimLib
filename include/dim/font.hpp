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

#include <string>
#include <memory>

#include "dim/texture.hpp"

namespace dim
{

  class Font
  {
    struct Glyph
    {
      GLubyte *map;
      size_t width;
      size_t heightAboveBaseLine;
      size_t advance;
    };

    Glyph d_glyphs[128];

    size_t d_heightAboveBaseLine;
    size_t d_heightBelowBaseLine;

    static bool s_initialized;

  public:
    Font(std::string filename, size_t maxSize);

    Texture<GLubyte> generateTexture(std::string const &text, bool centered, size_t width, size_t height, Filtering filter) const;
    //GLuint letter(size_t ch);

  private:
    void generateCharMap(size_t ch);
    GLubyte *scale(GLubyte *textMap, size_t oldWidth, size_t oldHeight, size_t newWidth, size_t newHeight) const;

    static void initialize();
  };


}
#endif
