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
#include <vector>

#include "dim/core/texture.hpp"

namespace dim
{

  class Font
  {
    struct Glyph
    {
      //std::shared_ptr<GLubyte> map;
      uint width;
      uint advance;
      glm::ivec2 origin;
    };
    
    Texture<GLubyte> d_fontMap;

    Glyph d_glyphs[128];

    uint d_heightAboveBaseLine;
    uint d_heightBelowBaseLine;

    static bool s_initialized;

  public:
    Font(std::string filename, uint maxSize);

    Texture<GLubyte> generateTexture(std::string const &text, bool centered, uint width, uint height, Filtering filter) const;
    Texture<GLubyte> const &map() const;
    
    uint height() const;
    
    glm::ivec2 const &origin(unsigned char ch) const;
    uint width(unsigned char ch) const;
    uint advance(unsigned char ch) const;

  private:
    std::vector<GLubyte> scale(std::vector<GLubyte> const &textMap, uint oldWidth, uint oldHeight, uint newWidth, uint newHeight) const;

    static void initialize();
    uint nextPowerOf2(uint number) const;
  };


}
#endif
