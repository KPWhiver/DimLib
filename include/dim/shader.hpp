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

#include "dim/camera.hpp"
#include "dim/light.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace dim
{
class Light;

class Shader
{
  std::shared_ptr<GLuint> d_id;
  std::shared_ptr<GLuint> d_fragmentId;
  std::shared_ptr<GLuint> d_vertexId;
  
  static Shader *s_activeShader;

  static glm::mat4 s_in_mat_model;
  //static glm::mat4 s_tmp_in_mat_model;

  static glm::mat3 s_in_mat_normal;
  //static glm::mat3 s_tmp_in_mat_normal;
    
  std::unordered_map<std::string, GLint> d_uniforms;
  
  std::vector<glm::mat4*> d_mat4List;
  std::vector<GLint> d_mat4Names;
  std::vector<glm::mat3*> d_mat3List;
  std::vector<GLint> d_mat3Names;
  std::vector<glm::vec4*> d_vec4List;
  std::vector<GLint> d_vec4Names;
  std::vector<glm::vec3*> d_vec3List;
  std::vector<GLint> d_vec3Names;
  std::vector<glm::vec2*> d_vec2List;
  std::vector<GLint> d_vec2Names;
  std::vector<int*> d_intList;
  std::vector<GLint> d_intNames;
  std::vector<Light*> d_lightList;
  std::vector<Camera*> d_cameraList;

  std::string d_vsName;
  std::string d_fsName;

public:
  //~Shader();

  Shader(std::string const &vsFile, std::string const &fsFile);

  void sendAtUse(glm::mat4* value, std::string const &variable);
  void sendAtUse(glm::mat3* value, std::string const &variable);
  void sendAtUse(glm::vec4* value, std::string const &variable);
  void sendAtUse(glm::vec3* value, std::string const &variable);
  void sendAtUse(glm::vec2* value, std::string const &variable);
  void sendAtUse(int* value, std::string const &variable);
  void sendAtUse(Light* light);
  void sendAtUse(Camera* camera);

  template<typename Type>
  void send(Type const &value, std::string const &variable);

  void send(glm::mat4 const &value, GLint variable) const;
  void send(glm::mat3 const &value, GLint variable) const;
  void send(glm::vec4 const &value, GLint variable) const;
  void send(glm::vec3 const &value, GLint variable) const;
  void send(glm::vec2 const &value, GLint variable) const;
  void send(float value, GLint variable) const;
  void send(int value, GLint variable) const;

  void use() const;

  static Shader &active();

  static glm::mat4 &in_mat_model();
  static glm::mat3 &in_mat_normal();

  void transformBegin();
  void transformEnd();

  GLuint id() const;

private:
  void preprocess(std::string const &filename);

  char* const returnshader(std::string const &filename) const;
  void check_compile(GLuint sha_ver, std::string const &file) const;
  void check_program(GLuint program) const;
  GLint uniform(std::string const &variable);

  //bool instanced;

};

//
// Using string
//
  template<typename Type>
  void Shader::send(Type const &value, std::string const &variable)
  {
    GLint loc = uniform(variable);
    send(value, loc);
  }

}

#endif
