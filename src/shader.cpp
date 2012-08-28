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

#include "dim/shader.hpp"
#include "dim/scanner.hpp"

#include <glm/gtc/matrix_inverse.hpp>

using namespace std;
using namespace glm;

namespace dim
{
  bool Shader::s_geometryShader(GLEW_ARB_geometry_shader4);
  bool Shader::s_tessellationShader(GLEW_ARB_tessellation_shader);
  bool Shader::s_computeShader(GLEW_ARB_compute_shader);


  Shader *Shader::s_activeShader = 0;

  mat4 Shader::s_modelMatrix = mat4(1.0);

  mat3 Shader::s_normalMatrix = mat3(1.0);

  GLint Shader::uniform(string const &variable)
  {
    auto it = d_uniforms.find(variable);

    if(it == d_uniforms.end())
    {
      GLint loc = glGetUniformLocation(*d_id, variable.c_str());

      if(loc == -1)
        log(d_filename, 0, LogType::warning, "Can`t find uniform " + variable);

      d_uniforms[variable] = loc;
      return loc;
    }

    return it->second;
  }

  void Shader::parseShader(string &vertexShader, string &fragmentShader, string &geometryShader, string &tessControlShader, string &tessEvalShader, string &computeShader) const
  {
    // open file
    //ifstream file(d_filename.c_str());
    //if(file.is_open() == 0)
    //  log(__FILE__, __LINE__, LogType::error, "Failed to open" + d_filename);

    // set up scanner
    stringstream unmatched;
    int unmatchedPos = unmatched.tellp();

    Scanner scanner;
    scanner.switchOstream(unmatched);
    scanner.switchIstream(d_filename);

    // set up stages
    int const numStages = 6;
    stringstream shaders[numStages];

    stringstream *output = 0;

    int fileNumber = 0;
    uint version = 0;

    try
    {
      // parse tokens
      while(int token = scanner.lex())
      {
        // --- I. things either inside or outside the header ---

        // check for things we couldn't match
        if(unmatchedPos != unmatched.tellp())
          log(scanner.filename(), scanner.lineNr(), LogType::warning, "Unexpected symbol(s): " + unmatched.str());

        // shader stage switch
        if(token >= Scanner::vertex && token < Scanner::vertex + 6)
        {
          output = &shaders[token - Scanner::vertex];

          if(version != 0)
            *output << "#version " << version << '\n';

          *output << "#line " << scanner.lineNr() + (version != 0) << ' ' << fileNumber;
          continue;
        }
        else if(token == Scanner::include)
        {
          ++fileNumber;
          if(output != 0)
            *output << "#line " << 0 << ' ' << fileNumber << '\n';
          continue;
        }
        else if(token == Scanner::endOfFile)
        {
          --fileNumber;
          if(output != 0)
            *output << "#line " << scanner.lineNr() + 1 << ' ' << fileNumber;
          continue;
        }
        // check for #version
        else if(token == Scanner::version)
        {
          if(version == 0)
          {
            token = scanner.lex();
            if(token == Scanner::number)
            {
              if(output == 0)
              {
                stringstream ss(scanner.matched());
                ss >> version;
              }
              else
                *output << "#version " << scanner.matched();
            }
            else
              log(scanner.filename(), scanner.lineNr(), LogType::warning, "Expected a number after #version instead of: " + scanner.matched());
          }
          else
            log(scanner.filename(), scanner.lineNr(), LogType::warning, "Ignoring #version since it has already been set");

          continue;
        }

        // --- II. things inside the header ---
        if(output == 0)
        {
          // inside header
          if(token == Scanner::glslWhitespace)
            continue;

          log(scanner.filename(), scanner.lineNr(), LogType::warning, "Can't parse: " + scanner.matched() + " shader stage has not yet been set");
        }
        // --- III. things outside the header ---
        else
        {
          // outside header
          switch(token)
          {
            case Scanner::number:
            case Scanner::glslToken:
            case Scanner::glslWhitespace:
              *output << scanner.matched();
              break;
            default:
              log(scanner.filename(), scanner.lineNr(), LogType::warning, "Unexpected symbol(s): " + scanner.matched());
          }
        }
      }
      // tokens parsed
    }
    catch(runtime_error &exc)
    {
      log(scanner.filename(), scanner.lineNr(), LogType::error, exc.what());
    }

    vertexShader = shaders[0].str();
    fragmentShader = shaders[1].str();
    geometryShader = shaders[2].str();
    tessControlShader = shaders[3].str();
    tessEvalShader = shaders[4].str();
    computeShader = shaders[5].str();
  }

  void Shader::checkCompile(GLuint shader, string const &stage) const
  {
    int const buffer_size = 512;
    char buffer[buffer_size] = { 0 };
    GLsizei length = 0;

    glGetShaderInfoLog(shader, buffer_size, &length, buffer);
    if(length > 0)
      log(d_filename, 0, LogType::note, "Compiling " + stage + ": " + buffer);
  }

  void Shader::checkProgram(GLuint program) const
  {
    const int buffer_size = 512;
    char buffer[buffer_size] = { 0 };
    GLsizei length = 0;

    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    if(length > 0)
      log(d_filename, 0, LogType::note, string("Linking: ") + buffer);

    glValidateProgram(program);
    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if(status == GL_FALSE)
      log(d_filename, 0, LogType::warning, string("Building failed: ") + buffer);
  }

  void Shader::compileShader(string const &file, string const &stage, shared_ptr<GLuint> &shader, GLuint shaderType)
  {
    shader.reset(new GLuint(glCreateShader(shaderType)), [&](GLuint *ptr)
                 {
                   glDetachShader(*d_id, *ptr);
                   glDeleteShader(*ptr);
                   delete ptr;
                 });

    char const *cfile = file.c_str();
    glShaderSource(*shader, 1, &cfile, 0);
    glCompileShader(*shader);
    checkCompile(*shader, stage);
    glAttachShader(*d_id, *shader);

  }

  Shader::Shader(string const &filename)
      :
          d_id(new GLuint(glCreateProgram()), [](GLuint *ptr)
               {
                 glDeleteProgram(*ptr);
                 delete ptr;
               }),
          d_filename(filename)
  {
    string standardVertex("#version 120\n attribute vec2 in_position;void main(){gl_Position = vec4(in_position, 0.0, 1.0);}");

    string vertex;
    string fragment;
    string geometry;
    string tessControl;
    string tessEval;
    string compute;

    parseShader(vertex, fragment, geometry, tessControl, tessEval, compute);

    /*cout << vertex << "\n------\n";
    cout << fragment << "\n------\n";
    cout << geometry << "\n------\n";
    cout << tessControl << "\n------\n";
    cout << tessEval << "\n------\n";
    cout << compute << "\n------------\n";*/

    if(vertex != "")
      compileShader(vertex, "vertex shader", d_vertexId, GL_VERTEX_SHADER);
    else
      compileShader(standardVertex, "vertex shader", d_vertexId, GL_VERTEX_SHADER);

    if(fragment != "")
      compileShader(fragment, "fragment shader", d_fragmentId, GL_FRAGMENT_SHADER);

    if(geometry != "")
    {
      if(s_geometryShader)
        compileShader(geometry, "geometry shader", d_geometryId, GL_GEOMETRY_SHADER);
      else
        log(d_filename, 0, LogType::warning, "Ignoring geometry shader because they are not supported on this graphics card");
    }

    if(tessControl != "")
    {
      if(s_tessellationShader)
        compileShader(tessControl, "tessellation control shader", d_tessControlId, GL_TESS_CONTROL_SHADER);
      else
        log(d_filename, 0, LogType::warning, "Ignoring tesselation control shader because they are not supported on this graphics card");
    }

    if(tessEval != "")
    {
      if(s_tessellationShader)
        compileShader(tessEval, "tesselation evaluation shader", d_tessEvalId, GL_TESS_EVALUATION_SHADER);
      else
        log(d_filename, 0, LogType::warning, "Ignoring tesselation evaluation shader because they are not supported on this graphics card");
    }

    if(compute != "")
    {
      if(s_computeShader)
        compileShader(compute, "compute shader", d_computeId, GL_COMPUTE_SHADER);
      else
        log(d_filename, 0, LogType::warning, "Ignoring compute shader because they are not supported on this graphics card");
    }

    glBindAttribLocation(*d_id, 0, "in_position");
    glBindAttribLocation(*d_id, 1, "in_normal");
    glBindAttribLocation(*d_id, 2, "in_texcoord0");
    glBindAttribLocation(*d_id, 3, "in_inst_placement");

    glLinkProgram(*d_id);
    checkProgram(*d_id);
  }

  Shader::~Shader()
  {
    if(s_activeShader == this)
      s_activeShader = 0;
  }

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

  mat4 &Shader::modelMatrix()
  {
    return s_modelMatrix;
  }

  mat3 &Shader::normalMatrix()
  {
    return s_normalMatrix;
  }

  void Shader::transformBegin()
  {
    //s_tmp_modelMatrix = s_modelMatrix;
    //s_tmp_normalMatrix = s_normalMatrix;

    send(s_modelMatrix, "modelMatrix");
    s_normalMatrix = inverseTranspose(mat3(s_modelMatrix));
    send(s_normalMatrix, "normalMatrix");
  }

  void Shader::transformEnd()
  {
    s_modelMatrix = mat4(1.0);
    s_normalMatrix = mat3(1.0);

    send(s_modelMatrix, "modelMatrix");
    send(s_normalMatrix, "normalMatrix");
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
