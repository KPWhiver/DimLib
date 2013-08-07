// camera.hpp
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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "dim/core/shader.hpp"

namespace dim
{

  class Camera
  {
    glm::vec3 d_coorFrom;
    glm::vec3 d_coorTo;
    glm::vec3 d_up;
    glm::mat4 d_view;
    glm::mat4 d_projection;
    float d_height, d_width;
    float d_fov;
    float d_zNear, d_zFar;
    
  public:
    enum projection
    {
      perspective,
      orthogonal,
      flat,
    };

    Camera();
    Camera(Camera::projection mode, float width, float height);
    Camera(Camera::projection mode, float width, float height, glm::vec3 coorFrom, glm::vec3 coorTo);

    void setFOV(float fov);
    void setZrange(float zNear, float zFar);
    void setCoorFrom(glm::vec3 coorFrom);
    void setCoorTo(glm::vec3 coorTo);
    void setCoorTo(float dir, float yDir);

    void setHeight(float height);
    void setWidth(float width);

    float fov() const;
    float height() const;
    float width() const;

    glm::mat4 const &viewMatrix() const;
    glm::mat4 const &projectionMatrix() const;

    void setAtShader(Shader const &shader, std::string const &viewMatrix = "viewMatrix", std::string const &projectionMatrix = "projectionMatrix");
    void editmode();
    bool frustum(float ox, float oy);

  private:
    void setProjection();
    void setView();

    Camera::projection d_mode;

    float A1, A2, B1, B2, D1, D2;
    
    bool d_changed;

  };

}

#endif
