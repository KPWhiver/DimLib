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

#include "dim/core/shader.hpp"
#include "dim/core/scanner.hpp"

using namespace std;
using namespace glm;

namespace dim
{
  namespace
  {
    pair<uint, string> processLayout(Scanner &scanner, ostream &output)
    {
      int nextToken = '(';
      bool nextIdentIsType = true;

      stringstream ss;

      uint location = 0;

      while(int token = scanner.lex())
      {
        if(token != nextToken && token != Token::whitespace)
          log(scanner.filename(), scanner.lineNr(), LogType::error, "Unexpected symbol(s): " + scanner.matched());

        // layout ( location = 0 ) attribute vec3 in_vertex
        // Token::layout '(' Token::location '=' Token::number ')' Token::in

        switch(token)
        {
          case '(':
            nextToken = Token::location;
            break;
          case ')':
            nextToken = Token::in;
            break;
          case Token::location:
            nextToken = '=';
            break;
          case '=':
            nextToken = Token::number;
            break;
          case Token::number:
            ss << scanner.matched();
            ss >> location;
            nextToken = ')';
            break;
          case Token::in:
            nextToken = Token::identifier;
            output << scanner.matched();
            break;
          case Token::identifier:
            output << scanner.matched();
            if(nextIdentIsType == true)
              nextIdentIsType = false;
            else
              return make_pair(location, scanner.matched());
            break;
          case Token::whitespace:
            output << scanner.matched();
            break;
          default:
            log(scanner.filename(), scanner.lineNr(), LogType::error, "Unexpected symbol(s): " + scanner.matched());
            return make_pair(0, "unknown");
        }
      }
      return make_pair(0, "unknown");
    }

    int processVersion(Scanner &scanner, stringstream *output, uint *globalVersion)
    {
      while(int token = scanner.lex())
      {
        if(token == Token::whitespace)
        {
          if(output != 0)
            *output << scanner.matched();
        }
        else if(token == Token::version)
        {
          token = scanner.lex();
          if(*globalVersion == 0)
          {
            if(token == Token::number)
            {
              if(output == 0)
              {
                stringstream ss(scanner.matched());
                ss >> *globalVersion;
              }
              else
                *output << "#version " << scanner.matched();
            }
            else
              log(scanner.filename(), scanner.lineNr(), LogType::error, "Expected a number after #version instead of: " + scanner.matched());
          }
          else
            log(scanner.filename(), scanner.lineNr(), LogType::note, "Ignoring #version since it has already been set");
        }
        else
          return token;
      }
      return 0;
    }
  }

  bool Shader::s_geometryShader(false);
  bool Shader::s_tessellationShader(false);
  bool Shader::s_computeShader(false);
  bool Shader::s_layout(false);
  bool Shader::s_separate(false);

  float Shader::s_maxTextureUnits(0);

  bool Shader::s_initialized(false);

  Shader const *Shader::s_activeShader = 0;

  void Shader::initialize()
  {
    s_initialized = true;

    s_geometryShader = GLEW_ARB_geometry_shader4;
    s_tessellationShader = GLEW_ARB_tessellation_shader;
    s_computeShader = GLEW_ARB_compute_shader;
    s_separate = GLEW_ARB_separate_shader_objects;
    s_layout = GLEW_ARB_explicit_attrib_location || s_separate;

    glGetFloatv(GL_MAX_TEXTURE_UNITS, &s_maxTextureUnits);
  }

  Shader const &Shader::defaultShader()
  {
    static Shader shader("defaultShader", "#version 120\n"
                         "vertex:\n"
                         "layout(location = dim_vertex) attribute vec4 in_position;\n"
                         "void main(){gl_Position = in_position;}\n"
                         "fragment:\n"
                         "void main(){gl_FragColor = vec4(1.0);}\n");

    return shader;
  }

  GLint Shader::uniform(string const &variable) const
  {
    auto it = d_uniforms->find(variable);

    if(it == d_uniforms->end())
    {
      GLint loc = glGetUniformLocation(*d_id, variable.c_str());

      if(loc == -1)
        log(d_filename, 0, LogType::warning, "Can`t find uniform " + variable);

      (*d_uniforms)[variable] = loc;
      return loc;
    }

    return it->second;
  }

  void Shader::parseShader(istream &input, string &vertexShader, string &fragmentShader, string &geometryShader, string &tessControlShader, string &tessEvalShader,
                           string &computeShader) const
  {
    // set up scanner
    stringstream unmatched;
    int unmatchedPos = unmatched.tellp();

    Scanner scanner(input, d_filename, unmatched);

    // set up stages
    int const numStages = 6;
    stringstream shaders[numStages];

    vector<pair<uint, string>> attributes;

    stringstream *output = 0;

    int fileNumber = 0;
    uint globalVersion = 0;

    bool parseLayout = s_layout;
    bool parseVersion = true;

    try
    {
      int token = 0;

      // parse tokens
      while(true)
      {
        // either get a token or use a already found one
        if(parseVersion == true && output == 0)
          token = processVersion(scanner, 0, &globalVersion);
        else if(parseVersion == false)
          token = scanner.lex();

        if(token == 0)
          break;

        parseVersion = false;

        // --- I. things either inside or outside the header ---

        // check for things we couldn't match
        if(unmatchedPos != unmatched.tellp())
          log(scanner.filename(), scanner.lineNr(), LogType::warning, "Unexpected symbol(s): " + unmatched.str());

        // shader stage switch
        if(token >= Token::vertex && token < Token::vertex + 6)
        {
          output = &shaders[token - Token::vertex];

          if(globalVersion != 0)
            *output << "#version " << globalVersion << '\n';

          token = processVersion(scanner, output, &globalVersion);
          parseVersion = true;

          if(s_layout)
            *output << "#extension GL_ARB_explicit_attrib_location : enable\n";

          if(s_separate)
            *output << "#extension GL_ARB_separate_shader_objects : enable\n";

          *output << "#line " << scanner.lineNr() + (globalVersion != 0) + s_layout + s_separate << ' ' << fileNumber << '\n';
          continue;
        }
        else if(token == Token::include)
        {
          ++fileNumber;
          if(output != 0)
            *output << "#line " << 0 << ' ' << fileNumber << '\n';
          continue;
        }
        else if(token == Token::endOfFile)
        {
          --fileNumber;
          if(output != 0)
            *output << "#line " << scanner.lineNr() + 1 << ' ' << fileNumber;
          continue;
        }

        // --- II. things inside the header ---
        if(output == 0)
        {
          // inside header
          if(token == Token::whitespace)
            continue;

          log(scanner.filename(), scanner.lineNr(), LogType::error, "Can't parse: " + scanner.matched() + ", shader stage has not yet been set");
        }
        // --- III. things outside the header ---
        else
        {
          if(token < 256)
          {
            *output << static_cast<unsigned char>(token);
            continue;
          }

          switch(token)
          {
            case Token::layout:
              if((not parseLayout || globalVersion < 140) && output == &shaders[0])
              {
                pair<uint, string> attrib = processLayout(scanner, *output);
                glBindAttribLocation(*d_id, attrib.first, attrib.second.c_str());
              }
              else
                *output << scanner.matched();
              break;
            case Token::version:
            case Token::number:
            case Token::whitespace:
            case Token::location:
            case Token::in:
            case Token::out:
            case Token::identifier:
              *output << scanner.matched();
              break;
            default:
              log(scanner.filename(), scanner.lineNr(), LogType::error, "Unexpected symbol(s): " + scanner.matched());
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
    char buffer[buffer_size] = {0};
    GLsizei length = 0;

    glGetShaderInfoLog(shader, buffer_size, &length, buffer);
    if(length > 0)
      log(d_filename, 0, LogType::note, buffer);
  }

  void Shader::checkProgram(GLuint program) const
  {
    const int buffer_size = 512;
    char buffer[buffer_size] = {0};
    GLsizei length = 0;

    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    if(length > 0)
      log(d_filename, 0, LogType::note, buffer);

    glValidateProgram(program);
    glGetProgramInfoLog(program, buffer_size, &length, buffer);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if(status == GL_FALSE)
      log(d_filename, 0, LogType::error, buffer);
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
          d_uniforms(new unordered_map<string, GLint>),
          d_filename(filename)
  {
    ifstream file(filename.c_str());
    if(not file.is_open())
    {
      log(__FILE__, __LINE__, LogType::error, "Failed to open file: " + filename);
      return;
    }

    init(file);
  }

  Shader::Shader(string const &name, string const &input)
      :
          d_id(new GLuint(glCreateProgram()), [](GLuint *ptr)
          {
            glDeleteProgram(*ptr);
            delete ptr;
          }),
          d_uniforms(new unordered_map<string, GLint>),
          d_filename(name)
  {
    istringstream sstream(input);
    init(sstream);
  }

  void Shader::init(istream &input)
  {
    if(s_initialized == false)
      initialize();

    string standardVertex("#version 120\n attribute vec2 in_position; void main(){gl_Position = vec4(in_position, 0.0, 1.0);}");

    string vertex;
    string fragment;
    string geometry;
    string tessControl;
    string tessEval;
    string compute;

    parseShader(input, vertex, fragment, geometry, tessControl, tessEval, compute);

#if 0
    cout << vertex << "\n------\n";
    cout << fragment << "\n------\n";
    cout << geometry << "\n------\n";
    cout << tessControl << "\n------\n";
    cout << tessEval << "\n------\n";
    cout << compute << "\n------------\n";
#endif

    if(vertex != "")
      compileShader(vertex, "vertex shader", d_vertexId, GL_VERTEX_SHADER);
    else
    {
      compileShader(standardVertex, "vertex shader", d_vertexId, GL_VERTEX_SHADER);
      glBindAttribLocation(*d_id, 0, "in_position");
    }

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

    //glBindAttribLocation(*d_id, 7, "in_position");
    //glBindAttribLocation(*d_id, 2, "in_normal");
    //glBindAttribLocation(*d_id, 3, "in_texcoord0");
    //glBindAttribLocation(*d_id, 4, "in_inst_placement");

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
    if(id() != active().id())
    {
      glUseProgram(*d_id);
      s_activeShader = const_cast<Shader*>(this);
    }
  }

  Shader const &Shader::active()
  {
    if(s_activeShader != 0)
      return *s_activeShader;
    else
      return defaultShader();
  }

// matrices

  void Shader::set(GLint variable, glm::mat4 const &value) const
  {
    glUniformMatrix4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat4x3 const &value) const
  {
    glUniformMatrix4x3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat4x2 const &value) const
  {
    glUniformMatrix4x2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3x4 const &value) const
  {
    glUniformMatrix3x4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3 const &value) const
  {
    glUniformMatrix3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3x2 const &value) const
  {
    glUniformMatrix3x2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2x4 const &value) const
  {
    glUniformMatrix2x4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2x3 const &value) const
  {
    glUniformMatrix2x3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2 const &value) const
  {
    glUniformMatrix2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

// arrays

  void Shader::set(GLint variable, glm::vec3 const *values, size_t size) const
  {
    glUniform3fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::vec2 const *values, size_t size) const
  {
    glUniform2fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::vec4 const *values, size_t size) const
  {
    glUniform4fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec3 const *values, size_t size) const
  {
    glUniform3iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec2 const *values, size_t size) const
  {
    glUniform2iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec4 const *values, size_t size) const
  {
    glUniform4iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec3 const *values, size_t size) const
  {
    glUniform3uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec2 const *values, size_t size) const
  {
    glUniform2uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec4 const *values, size_t size) const
  {
    glUniform4uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, GLfloat const *values, size_t size) const
  {
    glUniform1fv(variable, size, values);
  }

  void Shader::set(GLint variable, GLint const *values, size_t size) const
  {
    glUniform1iv(variable, size, values);
  }

  void Shader::set(GLint variable, GLuint const *values, size_t size) const
  {
    glUniform1uiv(variable, size, values);
  }

// single values

  void Shader::set(GLint variable, glm::vec3 const &value) const
   {
     glUniform3f(variable, value.x, value.y, value.z);
   }

   void Shader::set(GLint variable, glm::vec2 const &value) const
   {
     glUniform2f(variable, value.x, value.y);
   }

   void Shader::set(GLint variable, glm::vec4 const &value) const
   {
     glUniform4f(variable, value.x, value.y, value.z, value.w);
   }

   void Shader::set(GLint variable, glm::ivec3 const &value) const
   {
     glUniform3i(variable, value.x, value.y, value.z);
   }

   void Shader::set(GLint variable, glm::ivec2 const &value) const
   {
     glUniform2i(variable, value.x, value.y);
   }

   void Shader::set(GLint variable, glm::ivec4 const &value) const
   {
     glUniform4i(variable, value.x, value.y, value.z, value.w);
   }

   void Shader::set(GLint variable, glm::uvec3 const &value) const
   {
     glUniform3ui(variable, value.x, value.y, value.z);
   }

   void Shader::set(GLint variable, glm::uvec2 const &value) const
   {
     glUniform2ui(variable, value.x, value.y);
   }

   void Shader::set(GLint variable, glm::uvec4 const &value) const
   {
     glUniform4ui(variable, value.x, value.y, value.z, value.w);
   }

   void Shader::set(GLint variable, GLfloat value) const
   {
     glUniform1f(variable, value);
   }

   void Shader::set(GLint variable, GLint value) const
   {
     glUniform1i(variable, value);
   }

   void Shader::set(GLint variable, GLuint value) const
   {
     glUniform1ui(variable, value);
   }

  GLuint Shader::id() const
  {
    return *d_id;
  }

}
