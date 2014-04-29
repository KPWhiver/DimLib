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
  bool Shader::s_geometryShader(false);
  bool Shader::s_tessellationShader(false);
  bool Shader::s_computeShader(false);
  bool Shader::s_layout(false);
  bool Shader::s_separate(false);

  int Shader::s_maxTextureUnits(0);

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

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &s_maxTextureUnits);
  }

  Shader const &Shader::defaultShader()
  {
    /*static Shader shader("defaultShader", "#version 120\n"
                         "vertex:\n"
                         "layout(location = dim_vertex) attribute vec4 in_position;\n"
                         "void main(){gl_Position = in_position;}\n"
                         "fragment:\n"
                         "void main(){gl_FragColor = vec4(1.0);}\n");*/

    static Shader shader{fromString, "val_defaultShader", R"foo(
                   #version 120
                   uniform mat4 in_mat_projection;
                   uniform mat4 in_mat_view;
                   uniform mat4 in_mat_model;

                   attribute vec3 in_position;

                   void main()
                   {
                     gl_Position = in_mat_projection * in_mat_view * in_mat_model * vec4(in_position, 1);
                   }
                         )foo",
                         R"foo(
                   #version 120
                   void main()
                   {
                     gl_FragColor = vec4(1);
                   }
                         )foo"};
    static bool shaderInitialized = false;
    if(not shaderInitialized)
    {
      shader.bind("in_mat_projection", projectionMatrix);
      shader.bind("in_mat_view", viewMatrix);
      shader.bind("in_mat_model", modelMatrix);
      shader.bind("in_position", vertex);
      shaderInitialized = true;
    }

    return shader;
  }

  void Shader::bind(string const &variable, Uniform uniform)
  {
    if(d_uniformArray[uniform] != -1)
    {
      log(d_filename, 0, LogType::warning, "Rebinding the " + variable + " uniform ignored");
      return;
    }

    d_uniformArray[uniform] = findUniform(variable);
  }

  void Shader::bind(string const &variable, Attribute attribute)
  {
    if(d_attributeArray[attribute] != -1)
    {
      log(d_filename, 0, LogType::warning, "Rebinding the " + variable + " attribute ignored");
      return;
    }

    d_attributeArray[attribute] = findAttribute(variable);
  }

  GLint Shader::findAttribute(string const &variable) const
  {
    GLint loc = glGetAttribLocation(*d_id, variable.c_str());

    if(loc == -1)
      log(d_filename, 0, LogType::warning, "Can't find attribute " + variable);

    return loc;
  }

  GLint Shader::findUniform(string const &variable) const
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

  void Shader::checkCompile(GLuint shader, string const &filename, string const &stage) const
  {
    int const buffer_size = 512;
    char buffer[buffer_size] = {0};
    GLsizei length = 0;

    glGetShaderInfoLog(shader, buffer_size, &length, buffer);
    if(length > 0)
      log(filename, 0, LogType::note, buffer);
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
      throw log(d_filename, 0, LogType::error, buffer);
  }

  void Shader::compileShader(string const &input, string const &filename, string const &stage, shared_ptr<GLuint> &shader, GLuint shaderType)
  {
    shader.reset(new GLuint(glCreateShader(shaderType)), [&](GLuint *ptr)
    {
      glDetachShader(*d_id, *ptr);
      glDeleteShader(*ptr);
      delete ptr;
    });

    char const *cfile = input.c_str();
    glShaderSource(*shader, 1, &cfile, 0);
    glCompileShader(*shader);
    checkCompile(*shader, filename, stage);
    glAttachShader(*d_id, *shader);

  }

  void Shader::enableAttribute(Attribute attribute, Format format)
  {
    uint columns = format / 10;

    for(uint idx = 0; idx != columns; ++idx)
      glEnableVertexAttribArray(active().d_attributeArray[attribute] + idx);
  }

  void Shader::disableAttribute(Attribute attribute, Format format)
  {
    uint columns = format / 10;

    for(uint idx = 0; idx != columns; ++idx)
      glDisableVertexAttribArray(active().d_attributeArray[attribute] + idx);
  }

  void Shader::advanceAttributePerInstance(Attribute attribute, Format format, bool advance)
  {
    uint columns = format / 10;

    for(uint idx = 0; idx != columns; ++idx)
      glVertexAttribDivisor(active().d_attributeArray[attribute] + idx, advance);
  }

  Shader::Shader()
  {
    *this = defaultShader();
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
    d_uniformArray.fill(-1);
    d_attributeArray.fill(-1);
    parseYAML(filename);
  }

  Shader::Shader(string const &vertexFile, string const &fragmentFile,
                 string const &geometryFile, string const &tessControlFile,
                 string const &tessEvalFile, string const &computeFile)
    :
      d_id(new GLuint(glCreateProgram()), [](GLuint *ptr)
      {
        glDeleteProgram(*ptr);
        delete ptr;
      }),
      d_uniforms(new unordered_map<string, GLint>)
  {
    d_uniformArray.fill(-1);
    d_attributeArray.fill(-1);
    parseGLSLfiles(vertexFile, fragmentFile, geometryFile, tessControlFile, tessEvalFile, computeFile);
  }


  Shader::Shader(FromString, string const &name, string const &vertexInput, string const &fragmentInput)
    :
      d_id(new GLuint(glCreateProgram()), [](GLuint *ptr)
      {
        glDeleteProgram(*ptr);
        delete ptr;
      }),
      d_uniforms(new unordered_map<string, GLint>),
      d_filename(name)
  {
    d_uniformArray.fill(-1);
    d_attributeArray.fill(-1);
    parseGLSL(name + "_vertex", vertexInput, name + "_fragment", fragmentInput);
  }

  void Shader::parseYAMLuniform(YAML::Node const &document, Uniform uniform, string const &entryName)
  {
    if(document[entryName])
      d_uniformArray[uniform] = findUniform(document[entryName].as<string>());
  }

  void Shader::parseYAMLattribute(YAML::Node const &document, Attribute attribute, string const &entryName)
  {
    if(document[entryName])
      d_attributeArray[attribute] = findAttribute(document[entryName].as<string>().c_str());
  }

  void Shader::parseYAML(string const &filename)
  {
    try
    {
      // determine location of the file
      string directory;
      size_t locOfLastSlash = filename.find_last_of('/', string::npos);
      if(locOfLastSlash != string::npos)
        directory = filename.substr(0, locOfLastSlash + 1);

      YAML::Node document = YAML::LoadFile(filename);

      // Load the shader file
      string vertexFile;
      string fragmentFile;
      string geometryFile;
      string tessControlFile;
      string tessEvalFile;
      string computeFile;

      if(document["vertexFile"])
        vertexFile = directory + document["vertexFile"].as<string>();
      if(document["fragmentFile"])
        fragmentFile = directory + document["fragmentFile"].as<string>();
      if(document["geometryFile"])
        geometryFile = directory + document["geometryFile"].as<string>();
      if(document["tessControlFile"])
        tessControlFile = directory + document["tessControlFile"].as<string>();
      if(document["tessEvalFile"])
        tessEvalFile = directory + document["tessEvalFile"].as<string>();
      if(document["computeFile"])
        computeFile = directory + document["computeFile"].as<string>();

      parseGLSLfiles(vertexFile, fragmentFile, geometryFile, tessControlFile, tessEvalFile, computeFile);

      // Parser attribute names
      parseYAMLattribute(document, vertex, "vertex");
      parseYAMLattribute(document, normal, "normal");
      parseYAMLattribute(document, texCoord, "texCoord");
      parseYAMLattribute(document, binormal, "binormal");
      parseYAMLattribute(document, tangent, "tangent");
      parseYAMLattribute(document, instance, "instance");

      // Parse uniform names
      parseYAMLuniform(document, modelMatrix, "modelMatrix");
      parseYAMLuniform(document, viewMatrix, "viewMatrix");
      parseYAMLuniform(document, projectionMatrix, "projectionMatrix");
      parseYAMLuniform(document, normalMatrix, "normalMatrix");

      parseYAMLuniform(document, lightPosition,  "lightPosition");
      parseYAMLuniform(document, lightAmbientIntensity, "lightAmbientIntensity");
      parseYAMLuniform(document, lightColor, "lightColor");
      parseYAMLuniform(document, lightIntensity, "lightIntensity");
      parseYAMLuniform(document, highLightIntensity, "highLightIntensity");

      parseYAMLuniform(document, materialAmbient, "materialAmbient");
      parseYAMLuniform(document, materialDiffuse, "materialDiffuse");
      parseYAMLuniform(document, materialSpecular, "materialSpecular");
      parseYAMLuniform(document, materialShininess, "materialShininess");
      parseYAMLuniform(document, texture, "texture");
    }
    catch(exception &except)
    {
      throw log(__FILE__, __LINE__, LogType::error, except.what());
    }
  }

  string readFile(string const &filename)
  {
    ifstream file(filename.c_str());
    if(not file.is_open())
    {
      throw log(__FILE__, __LINE__, LogType::error, "Failed to open file: " + filename);
    }
    /*std::string contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    return contents;*/


    stringstream sstream;
    sstream << file.rdbuf();
    return sstream.str();
  }

  void Shader::linkShader()
  {
    glLinkProgram(*d_id);
    checkProgram(*d_id);
  }

  void Shader::parseGLSLfiles(string const &vertexFile, string const &fragmentFile,
                              string const &geometryFile, string const &tessControlFile,
                              string const &tessEvalFile, string const &computeFile)
  {
    string vertexInput;
    string fragmentInput;
    string geometryInput;
    string tessControlInput;
    string tessEvalInput;
    string computeInput;

    string globalFilename;

    if(vertexFile != "")
    {
      vertexInput = readFile(vertexFile);
      globalFilename += vertexFile + '|';
    }
    if(fragmentFile != "")
    {
      fragmentInput = readFile(fragmentFile);
      globalFilename += fragmentFile + '|';
    }
    else
    {
      throw log(__FILE__, __LINE__, LogType::error, "Every shader needs a fragment shader");
    }
    if(geometryFile != "")
    {
      geometryInput = readFile(geometryFile);
      globalFilename += geometryFile + '|';
    }
    if(tessControlFile != "")
    {
      tessControlInput = readFile(tessControlFile);
      globalFilename += tessControlFile + '|';
    }
    if(tessEvalFile != "")
    {
      tessEvalInput = readFile(tessEvalFile);
      globalFilename += tessEvalFile + '|';
    }
    if(computeFile != "")
    {
      computeInput = readFile(computeFile);
      globalFilename += computeFile + '|';
    }

    d_filename = globalFilename.substr(0, globalFilename.size() - 1);

    parseGLSL(vertexFile, vertexInput, fragmentFile, fragmentInput, geometryFile, geometryInput,
              tessControlFile, tessControlInput, tessEvalFile, tessEvalInput, computeFile, computeInput);
  }

  void Shader::parseGLSL(string const &vertexFile, string const &vertexInput,
                         string const &fragmentFile, string const &fragmentInput,
                         string const &geometryFile, string const &geometryInput,
                         string const &tessControlFile, string const &tessControlInput,
                         string const &tessEvalFile, string const &tessEvalInput,
                         string const &computeFile, string const &computeInput)
  {
    if(s_initialized == false)
      initialize();

    string standardVertex{"#version 120\n attribute vec2 in_position; void main(){gl_Position = vec4(in_position, 0.0, 1.0);}"};

#if 0
    cout << vertexInput << "\n------\n";
    cout << fragmentInput << "\n------\n";
    cout << geometryInput << "\n------\n";
    cout << tessControlInput << "\n------\n";
    cout << tessEvalInput << "\n------\n";
    cout << computeInput << "\n------------\n";
#endif

    if(vertexInput != "")
      compileShader(vertexInput, vertexFile, "vertex shader", d_vertexId, GL_VERTEX_SHADER);
    else
      compileShader(standardVertex, "val_novertex", "vertex shader", d_vertexId, GL_VERTEX_SHADER);

    if(fragmentInput != "")
      compileShader(fragmentInput, fragmentFile, "fragment shader", d_fragmentId, GL_FRAGMENT_SHADER);

    if(geometryInput != "")
    {
      if(s_geometryShader)
        compileShader(geometryInput, geometryFile, "geometry shader", d_geometryId, GL_GEOMETRY_SHADER);
      else
        log(geometryFile, 0, LogType::warning, "Ignoring geometry shader because they are not supported on this graphics card");
    }

    if(tessControlInput != "")
    {
      if(s_tessellationShader)
        compileShader(tessControlInput, tessControlFile, "tessellation control shader", d_tessControlId, GL_TESS_CONTROL_SHADER);
      else
        log(tessControlFile, 0, LogType::warning, "Ignoring tesselation control shader because they are not supported on this graphics card");
    }

    if(tessEvalInput != "")
    {
      if(s_tessellationShader)
        compileShader(tessEvalInput, tessEvalFile, "tesselation evaluation shader", d_tessEvalId, GL_TESS_EVALUATION_SHADER);
      else
        log(tessEvalFile, 0, LogType::warning, "Ignoring tesselation evaluation shader because they are not supported on this graphics card");
    }

    if(computeInput != "")
    {
      if(s_computeShader)
        compileShader(computeInput, computeFile, "compute shader", d_computeId, GL_COMPUTE_SHADER);
      else
        log(computeFile, 0, LogType::warning, "Ignoring compute shader because they are not supported on this graphics card");
    }

    linkShader();

    if(vertexInput == "")
      bind("in_position", vertex);
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

  void Shader::set(GLint variable, glm::mat4 const &value)
  {
    glUniformMatrix4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat4x3 const &value)
  {
    glUniformMatrix4x3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat4x2 const &value)
  {
    glUniformMatrix4x2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3x4 const &value)
  {
    glUniformMatrix3x4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3 const &value)
  {
    glUniformMatrix3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat3x2 const &value)
  {
    glUniformMatrix3x2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2x4 const &value)
  {
    glUniformMatrix2x4fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2x3 const &value)
  {
    glUniformMatrix2x3fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  void Shader::set(GLint variable, glm::mat2 const &value)
  {
    glUniformMatrix2fv(variable, 1, GL_FALSE, &value[0][0]);
  }

  // arrays

  void Shader::set(GLint variable, glm::vec3 const *values, size_t size)
  {
    glUniform3fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::vec2 const *values, size_t size)
  {
    glUniform2fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::vec4 const *values, size_t size)
  {
    glUniform4fv(variable, size, reinterpret_cast<GLfloat const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec3 const *values, size_t size)
  {
    glUniform3iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec2 const *values, size_t size)
  {
    glUniform2iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::ivec4 const *values, size_t size)
  {
    glUniform4iv(variable, size, reinterpret_cast<GLint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec3 const *values, size_t size)
  {
    glUniform3uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec2 const *values, size_t size)
  {
    glUniform2uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, glm::uvec4 const *values, size_t size)
  {
    glUniform4uiv(variable, size, reinterpret_cast<GLuint const *>(values));
  }

  void Shader::set(GLint variable, GLfloat const *values, size_t size)
  {
    glUniform1fv(variable, size, values);
  }

  void Shader::set(GLint variable, GLint const *values, size_t size)
  {
    glUniform1iv(variable, size, values);
  }

  void Shader::set(GLint variable, GLuint const *values, size_t size)
  {
    glUniform1uiv(variable, size, values);
  }

  // single values

  void Shader::set(GLint variable, glm::vec3 const &value)
  {
    glUniform3f(variable, value.x, value.y, value.z);
  }

  void Shader::set(GLint variable, glm::vec2 const &value)
  {
    glUniform2f(variable, value.x, value.y);
  }

  void Shader::set(GLint variable, glm::vec4 const &value)
  {
    glUniform4f(variable, value.x, value.y, value.z, value.w);
  }

  void Shader::set(GLint variable, glm::ivec3 const &value)
  {
    glUniform3i(variable, value.x, value.y, value.z);
  }

  void Shader::set(GLint variable, glm::ivec2 const &value)
  {
    glUniform2i(variable, value.x, value.y);
  }

  void Shader::set(GLint variable, glm::ivec4 const &value)
  {
    glUniform4i(variable, value.x, value.y, value.z, value.w);
  }

  void Shader::set(GLint variable, glm::uvec3 const &value)
  {
    glUniform3ui(variable, value.x, value.y, value.z);
  }

  void Shader::set(GLint variable, glm::uvec2 const &value)
  {
    glUniform2ui(variable, value.x, value.y);
  }

  void Shader::set(GLint variable, glm::uvec4 const &value)
  {
    glUniform4ui(variable, value.x, value.y, value.z, value.w);
  }

  void Shader::set(GLint variable, GLfloat value)
  {
    glUniform1f(variable, value);
  }

  void Shader::set(GLint variable, GLint value)
  {
    glUniform1i(variable, value);
  }

  void Shader::set(GLint variable, GLuint value)
  {
    glUniform1ui(variable, value);
  }

  GLuint Shader::id() const
  {
    return *d_id;
  }

}
