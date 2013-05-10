// dim.hpp
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

#ifndef dim_HPP
#define dim_HPP

#define GLFW_NO_GLU

#if defined(__APPLE_CC__)
 #include <GLEW/glew.h>
 #include <OpenGL/gl.h>
#else
 #include <GL/glew.h>
 #include <GL/gl.h>
#endif

#include <glm/glm.hpp>

#include <iosfwd>
#include <string>
#include <stdexcept>

namespace dim
{
  typedef unsigned int uint;

  enum class LogType
  {
    note,
    warning,
    error,
  };

  void setLogStream(std::ostream &outputStream);
  void setLogFile(std::string const &outputFile);

  std::runtime_error log(std::string const &file, int line, LogType type, std::string const &message);
}

#endif /* dim_HPP_ */
