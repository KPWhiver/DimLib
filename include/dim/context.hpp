// context.hpp
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

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <vector>

#include "dim/mesh.hpp"
#include "dim/font.hpp"
#include "dim/shader.hpp"

namespace dim
{
  class Context
  {
    Texture<GLubyte> d_butTexture;
    Texture<GLubyte> d_butHoverTexture;
    Texture<GLubyte> d_butDisableTexture;

    Font d_font;

    static bool s_initialized;

  public:
    Context(Texture<GLubyte> const &but, Texture<GLubyte> const &butHover, Texture<GLubyte> const &butDisable, Font const &font);

    Texture<GLubyte> const &buttonTexture() const;
    Texture<GLubyte> const &buttonHoverTexture() const;
    Texture<GLubyte> const &buttonDisableTexture() const;
    Font &font();
    Shader const &shader() const;

    Mesh const &mesh() const;
  };

}

#endif /* CONTEXT_HPP_ */
