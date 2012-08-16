// shader.cpp
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

#include <sstream>
/* We need cstring because that's what opengl wants... */
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "DIM/shader.hpp"
#include "DIM/scanner.hpp"

#include <glm/gtc/matrix_inverse.hpp>

using namespace std;
using namespace glm;

namespace dim
{

  Shader *Shader::s_activeShader = 0;

  mat4 Shader::s_in_mat_model = mat4(1.0);
//mat4 Shader::s_tmp_in_mat_model = mat4(1.0);

  mat3 Shader::s_in_mat_normal = mat3(1.0);
//mat3 Shader::s_tmp_in_mat_normal = mat3(1.0);

  GLint Shader::uniform(string const &variable)
  {
    auto it = d_uniforms.find(variable);

    if(it == d_uniforms.end())
    {
      GLint loc = glGetUniformLocation(*d_id, variable.c_str());

      if(loc == -1)
      {
        stringstream ss;
        ss << "Can`t find uniform " << variable << " in shader " << d_vsName << " \\ " << d_fsName;
        log(__FILE__, __LINE__, LogType::warning, ss.str());
      }

      d_uniforms[variable] = loc;
      return loc;
    }

    return it->second;
  }

  char* const Shader::returnshader(string const &filename) const
  {

    ifstream file(filename.c_str());
    if(file.is_open() == 0)
    {
      cerr << filename << " is empty\n";
    }
    else
    {
      stringstream output;

	    Scanner scanner(filename, "out");
	    scanner.switchOstream(output);
	
	    try
	    {
        while(int token = scanner.lex())
        {
          if(token == Scanner::include)
            output << "#line " << scanner.lineNr() << ' ' << 0 << '\n';
          else if(token == Scanner::endOfFile)
            output << "#line " << scanner.lineNr() << ' ' << 0 << '\n';
          else
            break;
        }
	    }
	    catch(runtime_error &exc)
	    {
	      log(__FILE__, __LINE__, LogType::warning, string("Compiling shader (file: ") + filename + ") failed: " + exc.what());
	    }
	
      string str = output.str();
      char* cstr;
      cstr = new char[str.size() + 1];
      strcpy(cstr, str.c_str());
      return cstr;
    }

    return 0;
  }

  void Shader::check_compile(GLuint sha_ver, string const &sha_file) const
  {
    const int buffer_size = 512;
    char buffer[buffer_size] = { 0 };
    GLsizei length = 0;

    glGetShaderInfoLog(sha_ver, buffer_size, &length, buffer);
    if(length > 0)
    {
      stringstream ss;
      ss << "Compiling shader " << sha_ver << " (file: " << sha_file << "): " << buffer;
      log(__FILE__, __LINE__, LogType::note, ss.str());
    }
  }

  void Shader::check_program(GLuint program) const
  {
    const int buffer_size = 512;
    char buffer[buffer_size] = { 0 };
    GLsizei length = 0;

    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    if(length > 0)
    {
      stringstream ss;
      ss << "Linking shader" << program << " (files: " << d_vsName << " \\ " << d_fsName << "): " << buffer;
      log(__FILE__, __LINE__, LogType::note, ss.str());
    }

    glValidateProgram(program);
    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if(status == GL_FALSE)
    {
      stringstream ss;
      ss << "Building shader " << program << " (files: " << d_vsName << " \\ " << d_fsName << ") failed:" << buffer;
      log(__FILE__, __LINE__, LogType::warning, ss.str());
    }
  }

  Shader::Shader(string const &vsFile, string const &fsFile)
      :
          d_id(new GLuint(glCreateProgram()), [&](GLuint *ptr)
               {
                 glDeleteProgram(*d_id);
                 delete ptr;
               }),
          d_fragmentId(new GLuint(glCreateShader(GL_FRAGMENT_SHADER)), [&](GLuint *ptr)
               {
                 glDetachShader(*d_id, *d_fragmentId);
                 glDeleteShader(*d_fragmentId);
                 delete ptr;
               }),
          d_vertexId(new GLuint(glCreateShader(GL_VERTEX_SHADER)), [&](GLuint *ptr)
               {
                 glDetachShader(*d_id, *d_vertexId);
                 glDeleteShader(*d_vertexId);
                 delete ptr;
               }),
          d_vsName(vsFile), d_fsName(fsFile)
  {
    //d_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //d_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char * fsText = returnshader(fsFile);
    glShaderSource(*d_fragmentId, 1, &fsText, 0);
    glCompileShader(*d_fragmentId);
    check_compile(*d_fragmentId, fsFile);
    delete[] fsText;

    const char * defines = "";

    const char * vsText[2] = { defines, returnshader(vsFile) };
    glShaderSource(*d_vertexId, 2, vsText, 0);
    glCompileShader(*d_vertexId);
    check_compile(*d_vertexId, vsFile);
    delete[] vsText[1];

    //d_id = glCreateProgram();
    glAttachShader(*d_id, *d_fragmentId);
    glAttachShader(*d_id, *d_vertexId);

    glBindAttribLocation(*d_id, 0, "in_position");
    glBindAttribLocation(*d_id, 1, "in_normal");
    glBindAttribLocation(*d_id, 2, "in_texcoord0");
    glBindAttribLocation(*d_id, 3, "in_inst_placement");

    glLinkProgram(*d_id);
    check_program(*d_id);
  }

  /*Shader::~Shader()
   {
   glDetachShader(d_id, d_fragmentShader);
   glDetachShader(d_id, d_vertexShader);

   glDeleteShader(d_fragmentShader);
   glDeleteShader(d_vertexShader);
   glDeleteProgram(d_id);
   }*/

  void Shader::use() const
  {
    if(s_activeShader == 0 || id() != active().id())
    {
      glUseProgram(*d_id);
      s_activeShader = const_cast<Shader*>(this);
    }

    for(size_t idx = 0; idx != d_mat4List.size(); ++idx)
      send(*d_mat4List[idx], d_mat4Names[idx]);

    for(size_t idx = 0; idx != d_mat3List.size(); ++idx)
      send(*d_mat3List[idx], d_mat3Names[idx]);

    for(size_t idx = 0; idx != d_vec4List.size(); ++idx)
      send(*d_vec4List[idx], d_vec4Names[idx]);

    for(size_t idx = 0; idx != d_vec3List.size(); ++idx)
      send(*d_vec3List[idx], d_vec3Names[idx]);

    for(size_t idx = 0; idx != d_vec2List.size(); ++idx)
      send(*d_vec2List[idx], d_vec2Names[idx]);

    for(size_t idx = 0; idx != d_intList.size(); ++idx)
      send(*d_intList[idx], d_intNames[idx]);

    for(size_t idx = 0; idx != d_lightList.size(); ++idx)
      d_lightList[idx]->send();

    for(size_t idx = 0; idx != d_cameraList.size(); ++idx)
      d_cameraList[idx]->send();

  }

  Shader &Shader::active()
  {
    return *s_activeShader;
  }

  mat4 &Shader::in_mat_model()
  {
    return s_in_mat_model;
  }

  mat3 &Shader::in_mat_normal()
  {
    return s_in_mat_normal;
  }

  void Shader::transformBegin()
  {
    //s_tmp_in_mat_model = s_in_mat_model;
    //s_tmp_in_mat_normal = s_in_mat_normal;

    send(s_in_mat_model, "in_mat_model");
    s_in_mat_normal = inverseTranspose(mat3(s_in_mat_model));
    send(s_in_mat_normal, "in_mat_normal");
  }

  void Shader::transformEnd()
  {
    s_in_mat_model = mat4(1.0);
    s_in_mat_normal = mat3(1.0);

    send(s_in_mat_model, "in_mat_model");
    send(s_in_mat_normal, "in_mat_normal");
  }

  void Shader::sendAtUse(glm::mat4* value, string const &variable)
  {
    d_mat4List.push_back(value);
    d_mat4Names.push_back(uniform(variable));
  }

  void Shader::sendAtUse(glm::mat3* value, string const &variable)
  {
    d_mat3List.push_back(value);
    d_mat3Names.push_back(uniform(variable));
  }

  void Shader::sendAtUse(glm::vec4* value, string const &variable)
  {
    d_vec4List.push_back(value);
    d_vec4Names.push_back(uniform(variable));
  }

  void Shader::sendAtUse(glm::vec3* value, string const &variable)
  {
    d_vec3List.push_back(value);
    d_vec3Names.push_back(uniform(variable));
  }

  void Shader::sendAtUse(glm::vec2* value, string const &variable)
  {
    d_vec2List.push_back(value);
    d_vec2Names.push_back(uniform(variable));
  }

  void Shader::sendAtUse(int* value, string const &variable)
  {
    d_intList.push_back(value);
    d_intNames.push_back(uniform(variable));
  }

  void Shader::sendAtUse(Light* light)
  {
    d_lightList.push_back(light);
  }

  void Shader::sendAtUse(Camera* camera)
  {
    d_cameraList.push_back(camera);
  }

//
// Using GLint
//
  void Shader::send(glm::mat4 const &value, GLint variable) const
  {
    glUniformMatrix4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::send(glm::mat3 const &value, GLint variable) const
  {
    glUniformMatrix3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::send(glm::vec3 const &value, GLint variable) const
  {
    glUniform3fv(variable, 1, &value[0]);
  }

  void Shader::send(glm::vec2 const &value, GLint variable) const
  {
    glUniform2fv(variable, 1, &value[0]);
  }

  void Shader::send(glm::vec4 const &value, GLint variable) const
  {
    glUniform4fv(variable, 1, &value[0]);
  }

  void Shader::send(float value, GLint variable) const
  {
    glUniform1f(variable, value);
  }

  void Shader::send(int value, GLint variable) const
  {
    glUniform1i(variable, value);
  }

  GLuint Shader::id() const
  {
    return *d_id;
  }

}
