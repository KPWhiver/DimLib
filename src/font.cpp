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
#include <algorithm>

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

Font::Font(string filename, uint maxSize)
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
	uint totalWidth(0);

	for (uint ch = 0; ch != 128; ++ch)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
		  log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

    if (FT_Get_Glyph(face->glyph, &glyphs[ch]))
      log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

    FT_Glyph_To_Bitmap(&glyphs[ch], ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(glyphs[ch]);

    d_heightAboveBaseLine = std::max(static_cast<uint>(std::max(0, glyph->top)), d_heightAboveBaseLine);
    d_heightBelowBaseLine = std::max(static_cast<uint>(std::max(0, glyph->bitmap.rows - glyph->top)), d_heightBelowBaseLine);
    
    totalWidth += glyph->bitmap.width;
	}
  uint height = d_heightAboveBaseLine + d_heightBelowBaseLine;

	// store glyphs
	uint mapHeight = nextPowerOf2((totalWidth / 1024 + 1) * height);
	uint mapWidth = totalWidth >= 1024 ? 1024 : nextPowerOf2(totalWidth);
	GLubyte *map = new GLubyte[mapHeight * mapWidth];
	fill_n(map, mapHeight * mapWidth, 0);
	
	ivec2 origin(0);
	
	for(uint ch = 0; ch != 128; ++ch)
	{
	  FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(glyphs[ch]);
    int verMove = d_heightAboveBaseLine - glyph->top;

	  d_glyphs[ch].width = glyph->bitmap.width;
	  d_glyphs[ch].advance = glyph->left;
	  d_glyphs[ch].origin = origin;

    for(uint x = 0; x != static_cast<uint>(glyph->bitmap.width); ++x)
    {
      for(uint y = 0; y != static_cast<uint>(glyph->bitmap.rows); ++y)
        map[(y + verMove + origin.y) * mapWidth + x + origin.x] = glyph->bitmap.buffer[y * d_glyphs[ch].width + x];
	  }
    FT_Done_Glyph(glyphs[ch]);
    
    if(origin.x + d_glyphs[ch].width >= mapWidth)
    {
      origin.x = 0;
      origin.y += height;
    }
    else
      origin.x += d_glyphs[ch].width;
	} 

	FT_Done_Face(face);
	
	d_fontMap = Texture<GLubyte>(map, Filtering::linear, NormalizedFormat::R8, mapWidth, mapHeight, true);
  delete map;
}

Texture<> Font::generateTexture(string const &text, bool centered, uint textureWidth, uint textureHeight, Filtering filter) const
{
  uint textHeight = d_heightAboveBaseLine + d_heightBelowBaseLine;
  uint textWidth = 0;
  uint unscaledTextureWidth = textureWidth * (static_cast<float>(textHeight) / textureHeight);

  for(uint letter = 0; letter != text.length(); ++letter)
  {
  	uint ch = text[letter];
    textWidth += d_glyphs[ch].width;
  }

  GLubyte *textMap = new GLubyte[textHeight * unscaledTextureWidth]{};

  uint xStart = 0;
  if(centered && textWidth < unscaledTextureWidth)
    xStart = (unscaledTextureWidth - textWidth) / 2;

  uint mapWidth = d_fontMap.width();

  for(uint letter = 0; letter != text.length(); ++letter)
  {
    uint ch = text[letter];
  	int horMove = d_glyphs[ch].advance;

    for(uint x = 0; x != d_glyphs[ch].width; ++x, ++xStart)
    {
      if(xStart + horMove >= unscaledTextureWidth)
        break;

      for(uint y = 0; y != textHeight; ++y)
      {
      	if(textMap[y * unscaledTextureWidth + xStart + horMove] == 0)
      		textMap[y * unscaledTextureWidth + xStart + horMove] = d_fontMap.buffer()[(y + d_glyphs[ch].origin.y) * mapWidth + x + d_glyphs[ch].origin.x];
      }
    }
  }

  GLubyte *texture = scale(textMap, unscaledTextureWidth, textHeight, textureWidth, textureHeight);

  Texture<> textTexture(texture, filter, NormalizedFormat::R8, textureWidth, textureHeight, false);

  delete[] textMap;
  delete[] texture;

  return textTexture;
}

Texture<GLubyte> const &Font::map() const
{
  return d_fontMap;
}

uint Font::height() const
{
  return d_heightAboveBaseLine + d_heightBelowBaseLine;
}

ivec2 const &Font::origin(unsigned char ch) const
{
  return d_glyphs[ch].origin;
}

uint Font::width(unsigned char ch) const
{
  return d_glyphs[ch].width;
}

uint Font::advance(unsigned char ch) const
{
  return d_glyphs[ch].advance;
}

uint Font::nextPowerOf2(uint number) const
{
  --number;
  number |= number >> 1;
  number |= number >> 2;
  number |= number >> 4;
  number |= number >> 8;
  number |= number >> 16;
  //if(sizeof(uint) == 8)
  //  number |= number >> 32;
  ++number;

  return number;
}

GLubyte *Font::scale(GLubyte *textMap, uint oldWidth, uint oldHeight, uint newWidth, uint newHeight) const
{

  GLubyte *texture = new GLubyte[newHeight * newWidth]{};
  float horScale = static_cast<float>(newWidth) / oldWidth;
  float verScale = static_cast<float>(newHeight) / oldHeight;

  for(uint x = 0; x != newWidth; ++x)
  {
    for(uint y = 0; y != newHeight; ++y)
    {
      uint x1(std::max(0, static_cast<int>(floor(x / horScale))));
      uint x2(std::min(x1 + 1, oldWidth - 1));
      uint y1(std::max(0, static_cast<int>(floor(y / verScale))));
      uint y2(std::min(y1 + 1, oldHeight - 1));

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
