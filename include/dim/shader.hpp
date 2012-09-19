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

#include "dim/texture.hpp"

namespace dim
{
class Light;

class Shader
{
  static bool s_geometryShader;
  static bool s_tessellationShader;
  static bool s_computeShader;
  static bool s_layout;
  static bool s_separate;
  static float s_maxTextureUnits;
  
  static bool s_initialized;
  
  static void initialize();

  static Shader const &defaultShader();

  std::shared_ptr<GLuint> d_id;
  std::shared_ptr<GLuint> d_fragmentId;
  std::shared_ptr<GLuint> d_vertexId;
  std::shared_ptr<GLuint> d_geometryId;
  std::shared_ptr<GLuint> d_tessControlId;
  std::shared_ptr<GLuint> d_tessEvalId;
  std::shared_ptr<GLuint> d_computeId;
  
  static Shader const *s_activeShader;
    
  mutable std::unordered_map<std::string, GLint> d_uniforms;
  
  std::string d_filename;

public:
  ~Shader();

  Shader(std::string const &filename);
  Shader(std::string const &name, std::string const &input);

  template<typename Type>
  GLint set(std::string const &variable, Type const &value) const;  
  
  template<typename Type>
  void set(std::string variable, Texture<Type> const &value, uint unit) const;
  
  template<typename Type>
  GLint set(std::string variable, Type const *values, size_t size) const;

  template<typename Type>
  void set(GLint variable, Type const &value) const;

  void set(GLint variable, glm::mat4 const &value) const;
  void set(GLint variable, glm::mat4x3 const &value) const;
  void set(GLint variable, glm::mat4x2 const &value) const;
  void set(GLint variable, glm::mat3x4 const &value) const;
  void set(GLint variable, glm::mat3 const &value) const;
  void set(GLint variable, glm::mat3x2 const &value) const;
  void set(GLint variable, glm::mat2x4 const &value) const;
  void set(GLint variable, glm::mat2x3 const &value) const;
  void set(GLint variable, glm::mat2 const &value) const;
  
  void set(GLint variable, glm::vec4 const *values, size_t size) const;
  void set(GLint variable, glm::vec3 const *values, size_t size) const;
  void set(GLint variable, glm::vec2 const *values, size_t size) const;
  void set(GLint variable, glm::ivec4 const *values, size_t size) const;
  void set(GLint variable, glm::ivec3 const *values, size_t size) const;
  void set(GLint variable, glm::ivec2 const *values, size_t size) const;
  void set(GLint variable, glm::uvec4 const *values, size_t size) const;
  void set(GLint variable, glm::uvec3 const *values, size_t size) const;
  void set(GLint variable, glm::uvec2 const *values, size_t size) const;
  void set(GLint variable, GLfloat const *values, size_t size) const;
  void set(GLint variable, GLint const *values, size_t size) const;
  void set(GLint variable, GLuint const *values, size_t size) const;
  
  void use() const;

  static Shader const &active();

  GLuint id() const;

  //static void initialize();

private:
  void parseShader(std::istream &input, std::string &vertexShader, std::string &fragmentShader, std::string &geometryShader, std::string &tessControlShader, std::string &tessEvalShader, std::string &computeShader) const;
  void compileShader(std::string const &file, std::string const &stage, std::shared_ptr<GLuint> &shader, GLuint shaderType);
  void checkCompile(GLuint shader, std::string const &stage) const;
  void checkProgram(GLuint program) const;
  GLint uniform(std::string const &variable) const;

  void init(std::istream &input);

  //bool instanced;

};

//
// Using string
//
  template<typename Type>
  void Shader::set(GLint variable, Type const &value) const
  {
    set(variable, &value, 1);
  }

  template<typename Type>
  GLint Shader::set(std::string const &variable, Type const &value) const
  {
    GLint loc = uniform(variable);
    set(loc, value);
    return loc;
  }
  
  template<typename Type>
  GLint Shader::set(std::string variable, Type const *values, size_t size) const
  {
    GLint loc = uniform(variable);
    set(loc, values, size);
    return loc;
  }

  template<typename Type>
  void Shader::set(std::string variable, Texture<Type> const &texture, uint unit) const
  { 
    //TODO logica
    if(unit > s_maxTextureUnits)
    {
      std::stringstream ss;
      ss << "This graphics card does not support " << unit << " texture units";
      log(__FILE__, __LINE__, LogType::warning, ss.str());
    }

    uint textureUnit = GL_TEXTURE0 + unit;

    glActiveTexture(textureUnit);
    set(variable, static_cast<int>(unit));
    texture.bind();
  }
}

#endif
