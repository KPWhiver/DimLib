// font.cpp
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
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "dim/font.hpp"

using namespace std;
using namespace glm;

namespace dim
{
namespace
{
  //this is here to avoid freetype inclusion in the font.hpp
  FT_Library g_library;
}


bool Font::s_initialized(false);

void Font::initialize()
{
  s_initialized = true;

  int error = FT_Init_FreeType(&g_library);

  if(error)
    log(__FILE__, __LINE__, LogType::error, "Failed to initialize FreeType2");
}

Font::Font(string filename, size_t maxSize)
:
    d_heightAboveBaseLine(0),
    d_heightBelowBaseLine(0)
{
  if(s_initialized == false)
    initialize();

  FT_Face face;
	int error = FT_New_Face(g_library, filename.c_str(), 0, &face);

	if(error == FT_Err_Unknown_File_Format)
	  log(__FILE__, __LINE__, LogType::error, "File: " + filename + " has a unsupported file format");
	else if (error)
	  log(__FILE__, __LINE__, LogType::error, "Failed to read file: " + filename);

	FT_Set_Pixel_Sizes(face, maxSize, maxSize);

	// load glyphs
	FT_Glyph glyphs[128];

	for (size_t ch = 0; ch != 128; ++ch)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
		  log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

    if (FT_Get_Glyph(face->glyph, &glyphs[ch]))
      log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

    FT_Glyph_To_Bitmap(&glyphs[ch], ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(glyphs[ch]);

    d_heightAboveBaseLine = std::max(static_cast<size_t>(std::max(0, glyph->top)), d_heightAboveBaseLine);
    d_heightBelowBaseLine = std::max(static_cast<size_t>(std::max(0, glyph->bitmap.rows - glyph->top)), d_heightBelowBaseLine);
	}
  size_t height = d_heightAboveBaseLine + d_heightBelowBaseLine;

	// store glyphs
	for(size_t ch = 0; ch != 128; ++ch)
	{
	  FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(glyphs[ch]);
    int verMove = d_heightAboveBaseLine - glyph->top;

    if(glyph->bitmap.width > 0)
      d_glyphs[ch].map.reset(new GLubyte[glyph->bitmap.width * height]{0});

	  d_glyphs[ch].width = glyph->bitmap.width;
	  d_glyphs[ch].advance = glyph->left;

    for(size_t x = 0; x != static_cast<size_t>(glyph->bitmap.width); ++x)
    {
      for(size_t y = 0; y != static_cast<size_t>(glyph->bitmap.rows); ++y)
        d_glyphs[ch].map.get()[(y + verMove) * d_glyphs[ch].width + x] = glyph->bitmap.buffer[y * d_glyphs[ch].width + x];
	  }
    FT_Done_Glyph(glyphs[ch]);
	}

	FT_Done_Face(face);
}

Texture<> Font::generateTexture(string const &text, bool centered, size_t textureWidth, size_t textureHeight, Filtering filter) const
{
  size_t textHeight = d_heightAboveBaseLine + d_heightBelowBaseLine;
  size_t textWidth = 0;
  size_t unscaledTextureWidth = textureWidth * (static_cast<float>(textHeight) / textureHeight);

  for(size_t letter = 0; letter != text.length(); ++letter)
  {
  	size_t ch = text[letter];
    textWidth += d_glyphs[ch].width;
  }

  GLubyte *textMap = new GLubyte[textHeight * unscaledTextureWidth]{0};

  size_t xStart = 0;
  if(centered && textWidth < unscaledTextureWidth)
    xStart = (unscaledTextureWidth - textWidth) / 2;

  for(size_t letter = 0; letter != text.length(); ++letter)
  {
    size_t ch = text[letter];
  	int horMove = d_glyphs[ch].advance;

    for(size_t x = 0; x != d_glyphs[ch].width; ++x, ++xStart)
    {
      if(xStart + horMove >= unscaledTextureWidth)
        break;

      for(size_t y = 0; y != textHeight; ++y)
      {
      	if(textMap[y * unscaledTextureWidth + xStart + horMove] == 0)
      		textMap[y * unscaledTextureWidth + xStart + horMove] = d_glyphs[ch].map.get()[y * d_glyphs[ch].width + x];
      }
    }
  }

  GLubyte *texture = scale(textMap, unscaledTextureWidth, textHeight, textureWidth, textureHeight);

  Texture<> textTexture(texture, filter, Format::R8, textureWidth, textureHeight, false);

  delete[] textMap;
  delete[] texture;

  return textTexture;
}

GLubyte *Font::scale(GLubyte *textMap, size_t oldWidth, size_t oldHeight, size_t newWidth, size_t newHeight) const
{

  GLubyte *texture = new GLubyte[newHeight * newWidth]{0};
  float horScale = static_cast<float>(newWidth) / oldWidth;
  float verScale = static_cast<float>(newHeight) / oldHeight;

  for(size_t x = 0; x != newWidth; ++x)
  {
    for(size_t y = 0; y != newHeight; ++y)
    {
      size_t x1(std::max(0, static_cast<int>(floor(x / horScale))));
      size_t x2(std::min(x1 + 1, oldWidth - 1));
      size_t y1(std::max(0, static_cast<int>(floor(y / verScale))));
      size_t y2(std::min(y1 + 1, oldHeight - 1));

      GLubyte upLeft(textMap[y1 * oldWidth + x1]);
      GLubyte downLeft(textMap[y2 * oldWidth + x1]);
      GLubyte upRight(textMap[y1 * oldWidth + x2]);
      GLubyte downRight(textMap[y2 * oldWidth + x2]);

      vec2 uv(x / horScale, y / verScale);
      vec2 frac(fract(uv));
      texture[y * newWidth + x] = mix(mix(upLeft, upRight, frac.x), mix(downLeft, downRight, frac.x), frac.y);
    }
  }
  return texture;
}

}
