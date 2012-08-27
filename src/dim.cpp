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
#include <stdexcept>

#include <dim/copyptr.hpp>
#include <dim/dim.hpp>

using namespace std;

namespace dim
{
  namespace
  {
    CopyPtr<ostream> output(new ostream(cerr.rdbuf()));
  }


  void setLogFile(string const &outputFile)
  {
    ofstream file(outputFile.c_str());

    if(not file.is_open())
      throw runtime_error("Failed to logfile: " + outputFile + " for writing");

    setLogStream(file);
  }

  void setLogStream(ostream &outputStream)
  {
    output.reset(new ostream(outputStream.rdbuf()));
  }

  void log(string const &file, int line, LogType type, string const &message)
  {
    *output << "In " << file << ':' << line << ": ";

    switch(type)
    {
      case LogType::note:
        *output << "Note: ";
        break;
      case LogType::warning:
        *output << "Warning: ";
        break;
      case LogType::error:
        *output << "Error: ";
        break;
    }
    *output << message << '\n';

    if(type == LogType::error)
      throw runtime_error(message);

  }
}
