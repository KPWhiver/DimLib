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

#include "dim/font.hpp"

using namespace std;

namespace dim {

FT_Library Font::s_library;

bool Font::s_initialized(false);

void Font::initialize()
{
  s_initialized = true;

  int error = FT_Init_FreeType(&s_library);

  if(error)
    log(__FILE__, __LINE__, LogType::error, "Failed to initialize FreeType2");
}

Font::Font(string filename)
{
  if(s_initialized == false)
    initialize();

	int error = FT_New_Face(s_library, filename.c_str(), 0, &d_face);

	if(error == FT_Err_Unknown_File_Format)
	  log(__FILE__, __LINE__, LogType::error, "File: " + filename + " has a unsupported file format");
	else if (error)
	  log(__FILE__, __LINE__, LogType::error, "Failed to read file: " + filename);

	FT_Set_Char_Size(d_face, 64 * 64, 64 * 64, 96, 96);

	for (size_t ch = 0; ch != 128; ++ch)
		generateCharMap(ch);

	FT_Done_Face(d_face);

}

void Font::generateCharMap(size_t ch)
{
	//d_charTex[ch].reset(new TextureBuffer);

	if (FT_Load_Glyph(d_face, FT_Get_Char_Index(d_face, ch), FT_LOAD_DEFAULT))
	  log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

	// Hamdle to a glyph
	FT_Glyph glyph;
	if (FT_Get_Glyph(d_face->glyph, &glyph))
	  log(__FILE__, __LINE__, LogType::error, "Failed to load glyph");

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	d_glyphs[ch] = glyph;
}

Texture<GLubyte> Font::generateTexture(string const &text, size_t textureWidth, size_t textureHeight) const
{
  size_t heightAboveLine = 0;
  size_t heightBelowLine = 0;
  size_t textHeight = 0;
  size_t textWidth = 0;
  for(size_t letter = 0; letter != text.length(); ++letter)
  {
  	size_t ch = text[letter];
  	FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(d_glyphs[ch]);

    heightAboveLine = max(static_cast<size_t>(glyph->top), heightAboveLine);
    heightBelowLine = max(static_cast<size_t>(glyph->bitmap.rows - glyph->top), heightBelowLine);
    textWidth += glyph->bitmap.width;
  }

  textHeight = heightAboveLine + heightBelowLine;
  //height = nextPowerOf2(heightAboveLine + heightBelowLine);
  //width = nextPowerOf2(width);

  while(textureWidth < textWidth || textureHeight < textHeight)
  {
    textureWidth *= 2;
    textureHeight *= 2;
  }
  
  size_t xStart = (textureWidth - textWidth) / 2;
  //size_t yStart = (textureHeight - textHeight) / 2;

  GLubyte *textMap = new GLubyte[4 * textureHeight * textureWidth];
  
  for(size_t idx = 0; idx != 4 * textureHeight * textureWidth; idx += 4)
  {
    textMap[idx] = 0;
    textMap[idx + 1] = 0;
    textMap[idx + 2] = 0;
    textMap[idx + 3] = 0;
  }

  size_t xTexture = xStart;

  for(size_t letter = 0; letter != text.length(); ++letter)
  {
  	size_t ch = text[letter];
  	FT_BitmapGlyph glyph = reinterpret_cast<FT_BitmapGlyph>(d_glyphs[ch]);
  	int horMove = glyph->left;
  	int verMove = heightAboveLine - glyph->top;

    for(size_t x = 0; x != static_cast<size_t>(glyph->bitmap.width); ++x, ++xTexture)
    {
      for(size_t y = 0; y != static_cast<size_t>(glyph->bitmap.rows); ++y)
      {
      	if(textMap[2 * ((y + verMove) * textureWidth + xTexture + horMove) + 1] == 0)
      		textMap[2 * ((y + verMove) * textureWidth + xTexture + horMove) + 1] = glyph->bitmap.buffer[y * glyph->bitmap.width + x];
      }
    }
  }

  Texture<GLubyte> textTex(textMap, Filtering::bilinear, Format::RG8, textureWidth, textureHeight, false);

  delete[] textMap;

  return textTex;
}

size_t Font::nextPowerOf2(size_t number) const
{
	size_t ret = 2;
	while (ret < number)
		ret <<= 1;

	return ret;
}

//GLuint Font::letter(size_t ch)
//{
//	return d_charTex[ch]->texture();
//}

}
