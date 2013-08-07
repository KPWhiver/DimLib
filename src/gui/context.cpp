// context.cpp
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

#include "dim/gui/context.hpp"

#include "GLFW/glfw3.h"

using namespace std;
using namespace glm;

namespace dim
{
	bool Context::s_initialized = false;
	
	Context::Context(Texture<GLubyte> const &defaultTexture, Font const &font)
			:
         d_button(defaultTexture),
         d_buttonOverlay(Texture<>::zeroTexture()),
         d_menuTop(defaultTexture),
         d_menuMiddle(defaultTexture),
         d_menuBottom(defaultTexture),
         d_menuOverlayTop(Texture<>::zeroTexture()),
         d_menuOverlayMiddle(Texture<>::zeroTexture()),
         d_menuOverlayBottom(Texture<>::zeroTexture()),
         d_menuOverlaySubmenu(Texture<>::zeroTexture()),
         d_switch(defaultTexture),
         d_switchPressed(defaultTexture),
         d_switchOverlay(Texture<>::zeroTexture()),
				 d_font(font),
				 d_hoverColor(0.3, 0.3, 0.3, 0.0),
				 d_disabledColor(-0.3, -0.3, -0.3, 0.0)
	{

	}

  void Context::setButtonTexture(Texture<> const &button)
  {
    d_button = button;
  }

  void Context::setButtonOverlayTexture(Texture<> const &overlay)
  {
    d_buttonOverlay = overlay;
  }

  void Context::setMenuTextures(Texture<> const &menuTop, Texture<> const &menuMiddle, Texture<> const &menuBottom)
  {
    d_menuTop = menuTop;
    d_menuMiddle = menuMiddle;
    d_menuBottom = menuBottom;
  }

  void Context::setMenuOverlayTextures(Texture<> const &overlayTop, Texture<> const &overlayMiddle, Texture<> const &overlayBottom, Texture<> const &overlaySubmenu)
  {
    d_menuOverlayTop = overlayTop;
    d_menuOverlayMiddle = overlayMiddle;
    d_menuOverlayBottom = overlayBottom;
  }
  
  void Context::setSwitchTextures(Texture<> const &switchTexture, Texture<> const &switchPressed)
  {
    d_switch = switchTexture;
    d_switchPressed = switchPressed;
  }

  void Context::setSwitchOverlayTexture(Texture<> const &overlay)
  {
    d_switchOverlay = overlay;
  }

  void Context::setHoverColor(vec4 const &color)
  {
    d_hoverColor = color;
  }

  void Context::setDisabledColor(vec4 const &color)
  {
    d_disabledColor = color;
  }

	Mesh const &Context::mesh() const
	{
		static GLfloat interleaved[24]{0, 0,  0, 0,  0, 1,  0, 1,  1, 0,  1, 0,  1, 1,  1, 1,  1, 0,  1, 0,  0, 1,  0, 1};
		
		static Mesh mesh(interleaved, {{Attribute::vertex, Attribute::vec2}, {Attribute::texCoord, Attribute::vec2}}, 6, Mesh::triangle, Mesh::interleaved);
		
		return mesh;
	}

	Texture<GLubyte> const &Context::buttonTexture() const
	{
		return d_button;
	}

	Texture<GLubyte> const &Context::buttonOverlayTexture() const
	{
		return d_buttonOverlay;
	}

  Texture<GLubyte> const &Context::menuTopTexture() const
  {
    return d_menuTop;
  }

  Texture<GLubyte> const &Context::menuMiddleTexture() const
  {
    return d_menuMiddle;
  }

  Texture<GLubyte> const &Context::menuBottomTexture() const
  {
    return d_menuBottom;
  }

  Texture<GLubyte> const &Context::menuOverlayTopTexture() const
  {
    return d_menuOverlayTop;
  }

  Texture<GLubyte> const &Context::menuOverlayMiddleTexture() const
  {
    return d_menuOverlayMiddle;
  }

  Texture<GLubyte> const &Context::menuOverlayBottomTexture() const
  {
    return d_menuOverlayBottom;
  }

  Texture<GLubyte> const &Context::menuOverlaySubmenuTexture() const
  {
    return d_menuOverlaySubmenu;
  }
  
  Texture<GLubyte> const &Context::switchTexture() const
	{
		return d_switch;
	}
  
  Texture<GLubyte> const &Context::switchPressedTexture() const
	{
		return d_switchPressed;
	}

	Texture<GLubyte> const &Context::switchOverlayTexture() const
	{
		return d_switchOverlay;
	}
	
  glm::vec4 const &Context::hoverColor() const
  {
    return d_hoverColor;
  }

  glm::vec4 const &Context::disabledColor() const
  {
    return d_disabledColor;
  }

	Font const &Context::font() const
	{
	  return d_font;
	}

	Shader const &Context::shader() const
	{
	  static Shader paletShader("guiShader", "#version 120\n"
	                            "vertex:\n"
	                            "uniform mat4 in_mat_projection;\n uniform mat4 in_mat_view;\n uniform mat4 in_mat_model;\n"
	                            "layout(location = dim_vertex) attribute vec2 in_position;\n layout(location = dim_texCoord) attribute vec2 in_texcoord;\n"
	                            "varying vec2 pass_texcoord;\n"
	                            "void main(){pass_texcoord = in_texcoord;\n"
	                            "gl_Position = in_mat_projection * in_mat_model * in_mat_view * vec4(in_position, vec2(1.0));}\n"
	                            "fragment:\n"
	                            "uniform sampler2D in_texture;\n"
	                            "uniform sampler2D in_text;\n"
	                            "uniform vec4 in_color;\n"
	                            "varying vec2 pass_texcoord;\n"
	                            "void main(){gl_FragColor = texture2D(in_texture, pass_texcoord)\n"
	                            "                           - vec4(vec3(texture2D(in_text, pass_texcoord).r), 0.0) "
	                            "                           + in_color;}");

	  return paletShader;
	}
}
