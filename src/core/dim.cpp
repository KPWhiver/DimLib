// dim.cpp
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
#include <fstream>
#include <string>
#include <memory>

#include <dim/util/copyptr.hpp>
#include <dim/core/dim.hpp>
#include "dim/core/surface.hpp"

using namespace std;

namespace dim
{
  namespace
  {
    ostream output{cerr.rdbuf()};
    ofstream fileOutput;
  }

  void setLogFile(string const &outputFile)
  {
    fileOutput.open(outputFile.c_str());

    if(not fileOutput.is_open())
      throw runtime_error("Failed to open logfile: " + outputFile + " for writing");

    setLogStream(fileOutput);
  }

  void setLogStream(ostream &outputStream)
  {
    output.rdbuf(outputStream.rdbuf());
  }

  runtime_error log(string const &file, int line, LogType type, string const &message)
  {
    std::cerr << "In " << file << ':' << line << ": ";

    switch(type)
    {
      case LogType::note:
        std::cerr << "Note: ";
        break;
      case LogType::warning:
        std::cerr << "Warning: ";
        break;
      case LogType::error:
        std::cerr << "Error: ";
        break;
    }
    std::cerr << message << '\n';

    return runtime_error(message);
  }

  void logGLerror(std::string const &file, int line)
  {
    int error = glGetError();
    switch(error)
    {
      case GL_INVALID_ENUM:
        log(file, line, LogType::warning, "OpenGL reported an error: GL_INVALID_ENUM");
        break;
      case GL_INVALID_VALUE:
        log(file, line, LogType::warning, "OpenGL reported an error: GL_INVALID_VALUE");
        break;
      case GL_INVALID_OPERATION:
        log(file, line, LogType::warning, "OpenGL reported an error: GL_INVALID_OPERATION");
        break;
      case GL_OUT_OF_MEMORY:
        log(file, line, LogType::warning, "OpenGL reported an error: GL_OUT_OF_MEMORY");
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        log(file, line, LogType::warning, "OpenGL reported an error: GL_INVALID_FRAMEBUFFER_OPERATION");
        break;
      case 0:
        break;
      default:
        log(file, line, LogType::warning, "OpenGL reported an error: " + to_string(error));
        break;
    }
  }
}
