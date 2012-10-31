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
    Texture<GLubyte> d_button;
    Texture<GLubyte> d_buttonOverlay;
    
    Texture<GLubyte> d_menuTop;
    Texture<GLubyte> d_menuMiddle;
    Texture<GLubyte> d_menuBottom;
    Texture<GLubyte> d_menuOverlayTop;
    Texture<GLubyte> d_menuOverlayMiddle;
    Texture<GLubyte> d_menuOverlayBottom;
    Texture<GLubyte> d_menuOverlaySubmenu;
    
    Texture<GLubyte> d_switch;
    Texture<GLubyte> d_switchPressed;
    Texture<GLubyte> d_switchOverlay;

    Font d_font;

    glm::vec4 d_hoverColor;
    glm::vec4 d_disabledColor;

    static bool s_initialized;

  public:
    Context(Texture<GLubyte> const &defaultTexture, Font const &font);

    Texture<GLubyte> const &buttonTexture() const;
    Texture<GLubyte> const &buttonOverlayTexture() const;
    
    Texture<GLubyte> const &menuTopTexture() const;
    Texture<GLubyte> const &menuMiddleTexture() const;
    Texture<GLubyte> const &menuBottomTexture() const;
    Texture<GLubyte> const &menuOverlayTopTexture() const;
    Texture<GLubyte> const &menuOverlayMiddleTexture() const;
    Texture<GLubyte> const &menuOverlayBottomTexture() const;
    Texture<GLubyte> const &menuOverlaySubmenuTexture() const;
    
    Texture<GLubyte> const &switchTexture() const;
    Texture<GLubyte> const &switchPressedTexture() const;
    Texture<GLubyte> const &switchOverlayTexture() const;

    glm::vec4 const &hoverColor() const;
    glm::vec4 const &disabledColor() const;

    Font const &font() const;
    Shader const &shader() const;

    Mesh const &mesh() const;

    void setButtonTexture(Texture<> const &button);
    void setButtonOverlayTexture(Texture<> const &overlay);
    
    void setMenuTextures(Texture<> const &menuTop, Texture<> const &menuMiddle, Texture<> const &menuBottom);
    void setMenuOverlayTextures(Texture<> const &overlayTop, Texture<> const &overlayMiddle, Texture<> const &overlayBottom, Texture<> const &overlaySubmenu);

    void setSwitchTextures(Texture<> const &switchTexture, Texture<> const &switchPressed);
    void setSwitchOverlayTexture(Texture<> const &overlay);

    void setHoverColor(glm::vec4 const &color);
    void setDisabledColor(glm::vec4 const &color);
  };

}

#endif /* CONTEXT_HPP_ */
