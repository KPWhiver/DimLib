// windowsurface.hpp
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

#ifndef WINDOWSURFACE_HPP
#define WINDOWSURFACE_HPP

#include <string>
#include <functional>

#include <glm/glm.hpp>
#include "dim/core/surface.hpp"

namespace dim
{

	class WindowSurface
  {
    std::function<void()> d_destroyWindow;
		size_t d_height, d_width;

		glm::vec4 d_clearColor;
		float d_clearDepth;
		bool d_blending;

	public:
    template<typename Lambda>
    WindowSurface(size_t width, size_t height, Lambda const &init = []{}, std::function<void()> const &destroy = []{});

		WindowSurface(WindowSurface const &other) = delete;
		WindowSurface(WindowSurface &&tmp) = default;

		WindowSurface &operator=(WindowSurface const &other) = delete;
		WindowSurface &operator=(WindowSurface &&tmp) = default;

    ~WindowSurface();

		size_t height() const;
		size_t width() const;

		void setHeight(size_t newHeight);
		void setWidth(size_t newWidth);

		void setClearColor(glm::vec4 const &color);
		void setClearDepth(float depth);

		void setBlending(bool blending);

		void clear();

    void renderTo(bool clearBuffer = true);
    void renderToPart(size_t x, size_t y, size_t width, size_t height, bool clearBuffer);
	};

  template<typename Lambda>
  WindowSurface::WindowSurface(size_t width, size_t height, Lambda const &init, std::function<void()> const &destroy)
      :
        d_destroyWindow(destroy),
        d_height(height),
        d_width(width),
        d_clearDepth(0),
        d_blending(true)
  {
    init();

    //initialize glew
    GLenum err = glewInit();
    if(GLEW_OK != err)
      throw log(__FILE__, __LINE__, LogType::error, std::string("Failed to initialize GLEW: ") + reinterpret_cast<char const *>(glewGetErrorString(err)));

    //initialize opengl stuff
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    internal::setBlending(true);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glClampColor(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
  }

}

#endif
