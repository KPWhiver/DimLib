// shader.hpp
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

#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "dim/dim.hpp"

namespace dim
{
class Light;

class Shader
{
  static bool s_geometryShader;
  static bool s_tessellationShader;
  static bool s_computeShader;

  std::shared_ptr<GLuint> d_id;
  std::shared_ptr<GLuint> d_fragmentId;
  std::shared_ptr<GLuint> d_vertexId;
  std::shared_ptr<GLuint> d_geometryId;
  std::shared_ptr<GLuint> d_tessControlId;
  std::shared_ptr<GLuint> d_tessEvalId;
  std::shared_ptr<GLuint> d_computeId;
  
  static Shader const *s_activeShader;

  static glm::mat4 s_modelMatrix;
  static glm::mat3 s_normalMatrix;
    
  mutable std::unordered_map<std::string, GLint> d_uniforms;
  
  std::string d_filename;

public:
  ~Shader();

  Shader(std::string const &filename);

  template<typename Type>
  void set(std::string const &variable, Type const &value) const;

  void set(GLint variable, glm::mat4 const &value) const;
  void set(GLint variable, glm::mat3 const &value) const;
  void set(GLint variable, glm::vec4 const &value) const;
  void set(GLint variable, glm::vec3 const &value) const;
  void set(GLint variable, glm::vec2 const &value) const;
  void set(GLint variable, float value) const;
  void set(GLint variable, int value) const;

  void use() const;

  static Shader const &active();

  static glm::mat4 &modelMatrix();
  static glm::mat3 &normalMatrix();

  void transformBegin() const;
  void transformEnd() const;

  GLuint id() const;

  //static void initialize();

private:
  void parseShader(std::string &vertexShader, std::string &fragmentShader, std::string &geometryShader, std::string &tessControlShader, std::string &tessEvalShader, std::string &computeShader) const;
  void compileShader(std::string const &file, std::string const &stage, std::shared_ptr<GLuint> &shader, GLuint shaderType);
  void checkCompile(GLuint shader, std::string const &stage) const;
  void checkProgram(GLuint program) const;
  GLint uniform(std::string const &variable) const;

  //bool instanced;

};

//
// Using string
//
  template<typename Type>
  void Shader::set(std::string const &variable, Type const &value) const
  {
    GLint loc = uniform(variable);
    set(loc, value);
  }

}

#endif
