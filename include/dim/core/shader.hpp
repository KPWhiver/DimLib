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

#include <yaml-cpp/yaml.h>

#include "dim/core/texture.hpp"
#include "dim/core/buffer.hpp"

namespace dim
{
class Shader
{
  static bool s_geometryShader;
  static bool s_tessellationShader;
  static bool s_computeShader;
  static bool s_layout;
  static bool s_separate;
  static int s_maxTextureUnits;
  
  static bool s_initialized;
  
  static void initialize();

  std::shared_ptr<GLuint> d_id;
  std::shared_ptr<GLuint> d_fragmentId;
  std::shared_ptr<GLuint> d_vertexId;
  std::shared_ptr<GLuint> d_geometryId;
  std::shared_ptr<GLuint> d_tessControlId;
  std::shared_ptr<GLuint> d_tessEvalId;
  std::shared_ptr<GLuint> d_computeId;
 
  static Shader *s_activeShader;
    
  mutable std::shared_ptr<std::unordered_map<std::string, GLint>> d_uniforms;
  mutable std::shared_ptr<std::unordered_map<std::string, GLint>> d_attributes;
  
  std::string d_filename;

  enum class UniformStatus : GLint
  {
    notFound = -1,
    notInitialised = -2
  };

  constexpr static int const s_uniformArraySize = 14;
  std::array<GLint, s_uniformArraySize> d_uniformArray;

  constexpr static int const s_attributeArraySize = 6;
  std::array<GLint, s_attributeArraySize> d_attributeArray;

public:
  enum FromString
  {
    fromString
  };

  enum Attribute
  {
    vertex = 0,
    normal = 1,
    texCoord = 2,
    binormal = 3,
    tangent = 4,
    boneId = 5,
    boneWeight = 6,
    instance = 7
  };

  enum Uniform
  {
    modelMatrix = 0,
    viewMatrix = 1,
    projectionMatrix = 2,
    normalMatrix = 3,

    lightPosition = 4,
    lightAmbientIntensity = 5,
    lightColor = 6,
    lightIntensity = 7,
    highLightIntensity = 8,

    materialAmbient = 9,
    materialDiffuse = 10,
    materialSpecular = 11,
    materialShininess = 12,
    texture = 13
  };

  enum Format: uint
  {
    vec1 = 11,
    vec2 = 12,
    vec3 = 13,
    vec4 = 14,
    mat2 = 22,
    mat2x3 = 23,
    mat2x4 = 24,
    mat3x2 = 32,
    mat3 = 33,
    mat3x4 = 34,
    mat4x2 = 42,
    mat4x3 = 43,
    mat4 = 44
  };

  ~Shader();

  Shader();
  explicit Shader(std::string const &filename);
  Shader(std::string const &vertexFile, std::string const &fragmentFile,
         std::string const &geometryFile = "", std::string const &tessControlFile = "",
         std::string const &tessEvalFile = "", std::string const &computeFile = "");
  Shader(FromString, std::string const &name, std::string const &vertexInput, std::string const &fragmentInput);

  void bind(std::string const &variable, Uniform uniform);
  void bind(std::string const &variable, Attribute attribute);

  template<typename Type>
  static GLint set(std::string const &variable, Type const &value);
  
  template<typename Type>
  static void set(std::string const &variable, Texture<Type> const &value, uint unit);

  static void enableAttribute(Attribute attribute, Format format);
  static void disableAttribute(Attribute attribute, Format format);
  static void advanceAttributePerInstance(Attribute attribute, Format format, bool advance);
  static void enableAttribute(std::string const &attribute, Format format);
  static void disableAttribute(std::string const &attribute, Format format);
  static void advanceAttributePerInstance(std::string const &attribute, Format format, bool advance);
  static void enableAttribute(GLint attribute, Format format);
  static void disableAttribute(GLint attribute, Format format);
  static void advanceAttributePerInstance(GLint attribute, Format format, bool advance);

  template<typename Type>
  static void set(Attribute attribute, Buffer<Type> const &value, Format format, uint floatStartOffset = 0, uint floatStride = 0);
  template<typename Type>
  static GLint set(std::string const &attribute, Buffer<Type> const &value, Format format, uint floatStartOffset = 0, uint floatStride = 0);
  template<typename Type>
  static void set(GLint attribute, Buffer<Type> const &value, Format format, uint floatStartOffset = 0, uint floatStride = 0);

  template<typename Type>
  static GLint set(std::string const &variable, Type const *values, size_t size);

  template<typename Type>
  static void set(Uniform uniform, Type const &value);

  static void set(GLint variable, glm::mat4 const &value);
  static void set(GLint variable, glm::mat4x3 const &value);
  static void set(GLint variable, glm::mat4x2 const &value);
  static void set(GLint variable, glm::mat3x4 const &value);
  static void set(GLint variable, glm::mat3 const &value);
  static void set(GLint variable, glm::mat3x2 const &value);
  static void set(GLint variable, glm::mat2x4 const &value);
  static void set(GLint variable, glm::mat2x3 const &value);
  static void set(GLint variable, glm::mat2 const &value);
  
  static void set(GLint variable, glm::vec4 const *values, size_t size);
  static void set(GLint variable, glm::vec3 const *values, size_t size);
  static void set(GLint variable, glm::vec2 const *values, size_t size);
  static void set(GLint variable, glm::ivec4 const *values, size_t size);
  static void set(GLint variable, glm::ivec3 const *values, size_t size);
  static void set(GLint variable, glm::ivec2 const *values, size_t size);
  static void set(GLint variable, glm::uvec4 const *values, size_t size);
  static void set(GLint variable, glm::uvec3 const *values, size_t size);
  static void set(GLint variable, glm::uvec2 const *values, size_t size);
  static void set(GLint variable, GLfloat const *values, size_t size);
  static void set(GLint variable, GLint const *values, size_t size);
  static void set(GLint variable, GLuint const *values, size_t size);
  
  static void set(GLint variable, glm::vec4 const &value);
  static void set(GLint variable, glm::vec3 const &value);
  static void set(GLint variable, glm::vec2 const &value);
  static void set(GLint variable, glm::ivec4 const &value);
  static void set(GLint variable, glm::ivec3 const &value);
  static void set(GLint variable, glm::ivec2 const &value);
  static void set(GLint variable, glm::uvec4 const &value);
  static void set(GLint variable, glm::uvec3 const &value);
  static void set(GLint variable, glm::uvec2 const &value);
  static void set(GLint variable, GLfloat value);
  static void set(GLint variable, GLint value);
  static void set(GLint variable, GLuint value);

  void use() const;

  static Shader const &active();

  GLuint id() const;

  static Shader const &defaultShader();

private:
  static Shader &activePrivate();

  void compileShader(std::string const &input, std::string const &filename, std::string const &stage, std::shared_ptr<GLuint> &shader, GLuint shaderType);
  void checkCompile(GLuint shader, std::string const &filename, std::string const &stage) const;
  void checkProgram(GLuint program) const;
  GLint findUniform(std::string const &variable) const;
  GLint findAttribute(std::string const &variable) const;

  void parseGLSL(std::string const &vertexFile, std::string const &vertexInput,
                 std::string const &fragmentFile, std::string const &fragmentInput,
                 std::string const &geometryFile = "", std::string const &geometryInput = "",
                 std::string const &tessControlFile = "", std::string const &tessControlInput = "",
                 std::string const &tessEvalFile = "", std::string const &tessEvalInput = "",
                 std::string const &computeFile = "", std::string const &computeInput = "");
  void parseGLSLfiles(std::string const &vertexFile, std::string const &fragmentFile,
                      std::string const &geometryFile, std::string const &tessControlFile,
                      std::string const &tessEvalFile, std::string const &computeFile);
  void linkShader();

  void parseYAML(std::string const &filename);

  void parseYAMLuniform(YAML::Node const &document, Shader::Uniform uniform, std::string const &entryName);
  void parseYAMLattribute(YAML::Node const &document, Shader::Attribute attribute, std::string const &entryName);
};

//
// Using string
//
  template<typename Type>
  GLint Shader::set(std::string const &variable, Type const &value)
  {
    GLint loc = active().findUniform(variable);
    set(loc, value);
    return loc;
  }

  template<typename Type>
  GLint Shader::set(std::string const &variable, Type const *values, size_t size)
  {
    GLint loc = active().findUniform(variable);
    set(loc, values, size);
    return loc;
  }

  template<typename Type>
  void Shader::set(Uniform uniform, Type const &value)
  {
    if(active().d_uniformArray[uniform] == static_cast<GLint>(UniformStatus::notInitialised))
    {
      activePrivate().d_uniformArray[uniform] = static_cast<GLint>(UniformStatus::notFound);
      log(active().d_filename, 0, LogType::warning, "Uniform " + std::to_string(uniform) + " has not been bound yet");
    }

    set(active().d_uniformArray[uniform], value);
  }

  template<typename Type>
  void Shader::set(std::string const &variable, Texture<Type> const &texture, uint unit)
  { 
    //TODO logica
    if(static_cast<int>(unit) > s_maxTextureUnits)
      log(__FILE__, __LINE__, LogType::warning, "This graphics card does not support " + std::to_string(unit) + " texture units");

    uint textureUnit = GL_TEXTURE0 + unit;

    glActiveTexture(textureUnit);
    set(variable, static_cast<int>(unit));
    texture.bind();
  }

  template<typename Type>
  inline void Shader::set(Attribute attribute, Buffer<Type> const &value, Format format, uint floatStartOffset, uint floatStride)
  {
    if(active().d_attributeArray[attribute] == static_cast<GLint>(UniformStatus::notInitialised))
    {
      activePrivate().d_attributeArray[attribute] = static_cast<GLint>(UniformStatus::notFound);
      log(active().d_filename, 0, LogType::warning, "Attribute " + std::to_string(attribute) + " has not been bound yet");
    }

    set(active().d_attributeArray[attribute], value, format, floatStartOffset, floatStride);
  }

  template<typename Type>
  inline GLint Shader::set(std::string const &attribute, Buffer<Type> const &value, Format format, uint floatStartOffset, uint floatStride)
  {
    GLint loc = active().findAttribute(attribute);
    set(loc, value, format, floatStartOffset, floatStride);
    return loc;
  }

  template<typename Type>
  inline void Shader::set(GLint attribute, Buffer<Type> const &value, Format format, uint floatStartOffset, uint floatStride)
  {
    uint rows = format % 10;
    uint columns = format / 10;

    // If this is a tightly packed matrix
    if(floatStride == 0 && columns > 1)
      floatStride = rows * columns;

    for(uint idx = 0; idx != columns; ++idx)
      glVertexAttribPointer(attribute + idx, rows, GL_FLOAT, GL_FALSE, floatStride * sizeof(GLfloat), reinterpret_cast<void*>((floatStartOffset + idx * rows) * sizeof(GLfloat)));
  }

  namespace internal
  {
    inline uint formatSize(Shader::Format format)
    {
      //mat2x3: 2 columns, 3 rows
      uint rows = format % 10;
      uint columns = format / 10;

      return columns * rows;
    }
  }
}

#endif
