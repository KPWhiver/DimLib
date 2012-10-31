// window.cpp
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

#include <GL/glew.h> 
#include "GL/glfw.h"

#include <string>
#include <iostream>
#include <stdexcept>

#include "dim/window.hpp"
#include "dim/mesh.hpp"
#include "dim/font.hpp"
#include "dim/texture.hpp"
#include "dim/surface.hpp"

using namespace std;
using namespace glm;

namespace dim
{
  bool Window::s_set(false);

  size_t Window::height() const
  {
    return d_height;
  }

  size_t Window::width() const
  {
    return d_width;
  }

  Mouse const &Window::mouse() const
  {
    return d_mouse;
  }

  double Window::fps() const
  {
    return d_fps;
  }

  double Window::fpsFactor() const
  {
    if(d_maxRunFps == 0 || d_fps < 10)
      return 1;

    return d_maxRunFps / d_fps;
  }

  void Window::setMaxRunFps(size_t fps)
  {
    d_maxRunFps = fps;
  }

  void Window::swapBuffers()
  {
    double currentTime = glfwGetTime();

    d_fps = 1 / (currentTime - d_startTime);

    d_startTime = currentTime;

    glfwSwapBuffers();

    d_mouse.update();

    int error = glGetError();
    switch(error)
    {
      case GL_INVALID_ENUM:
        log(__FILE__, __LINE__, LogType::warning, "OpenGL reported an error: GL_INVALID_ENUM");
        break;
      case GL_INVALID_VALUE:
        log(__FILE__, __LINE__, LogType::warning, "OpenGL reported an error: GL_INVALID_VALUE");
        break;
      case GL_INVALID_OPERATION:
        log(__FILE__, __LINE__, LogType::warning, "OpenGL reported an error: GL_INVALID_OPERATION");
        break;
      case GL_OUT_OF_MEMORY:
        log(__FILE__, __LINE__, LogType::warning, "OpenGL reported an error: GL_OUT_OF_MEMORY");
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        log(__FILE__, __LINE__, LogType::warning, "OpenGL reported an error: GL_INVALID_FRAMEBUFFER_OPERATION");
        break;
      case 0:
        break;
      default:
        stringstream ss;
        ss << "OpenGL reported an error: " << error;
        log(__FILE__, __LINE__, LogType::warning, ss.str());
        break;
    }
  }

  Window::Window(size_t width, size_t height, Window::Mode mode, string title)
      :
          d_maxRunFps(60),
          d_mode(mode)

  {
    if(s_set == true)
      return;

    s_set = true;

    //initialize glfw
    if(glfwInit() != GL_TRUE)
      log(__FILE__, __LINE__, LogType::error, "Failed to open a window");

    d_startTime = glfwGetTime();

    int screen = GLFW_WINDOW;

    if(mode == Window::fullscreen)
      screen = GLFW_FULLSCREEN;

    if(height == 0 || width == 0)
    {
      GLFWvidmode resolution;
      glfwGetDesktopMode(&resolution);
      d_height = resolution.Height;
      d_width = resolution.Width;
    }
    else
    {
      d_height = height;
      d_width = width;
    }

    //create new window
    if(glfwOpenWindow(d_width, d_height, 8, 8, 8, 8, 32, 0, screen) != GL_TRUE)
      log(__FILE__, __LINE__, LogType::error, "Failed to open a window");

    glfwSetWindowTitle(title.c_str());

    int realWidth;
    int realHeight;
    glfwGetWindowSize(&realWidth, &realHeight);

    d_width = realWidth;
    d_height = realHeight;

    //d_mouse.setRealSize(realWidth, realHeight);
    //glfwDisable(GLFW_MOUSE_CURSOR);

    //initialize glew
    GLenum err = glewInit();
    if(GLEW_OK != err)
      log(__FILE__, __LINE__, LogType::error, string("Failed to open a window: ") + reinterpret_cast<char const *>(glewGetErrorString(err)));

    //initialize opengl stuff
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    swapBuffers();
  }

  void Window::setCloseFunction(int (*closeFunction)())
  {
    glfwSetWindowCloseCallback(closeFunction);
  }

  void Window::setChangeSizeFunction(void (*changeSizeFunction)(int, int))
  {
    glfwSetWindowSizeCallback(changeSizeFunction);
  }

  void Window::enableVSync(bool enable)
  {
    glfwSwapInterval(enable);
  }

  Window::~Window()
  {
    glfwCloseWindow();
    glfwTerminate();
  }

  void Window::setClearColor(vec4 const &color)
  {
    d_clearColor = color;
  }

  void Window::renderTo()
  {
    renderToPart(0, 0, d_width, d_height, true);
  }

  void Window::renderToPart(size_t x, size_t y, size_t width, size_t height, bool clear)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(x, y, width, height);

    // If the last FBO is a pingpong buffer now is the time to swap those buffers
    if(SurfaceBase__::s_renderTarget != 0)
      SurfaceBase__::s_renderTarget->swapBuffers();

    SurfaceBase__::s_renderTarget = 0;

    // Clear the buffer before drawing
    if(clear)
    {
      if(d_clearColor != glm::vec4())
        glClearColor(d_clearColor.r, d_clearColor.g, d_clearColor.b, d_clearColor.a);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if(d_clearColor != glm::vec4())
        glClearColor(0, 0, 0, 0);
    }

    glColorMask(true, true, true, true);

  }

}

