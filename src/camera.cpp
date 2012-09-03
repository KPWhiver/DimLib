// camera.cpp
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


#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/intersect.hpp>

#include "dim/camera.hpp"

using namespace glm;

namespace dim
{

Camera::Camera()
{
	init(Camera::perspective, 0, 0, vec3(0.0f), vec3(1.0f, 0.0f, 0.0f));
}

Camera::Camera(Camera::projection mode, float width, float height)
{
  init(mode, width, height, vec3(0.0f), vec3(1.0f, 0.0f, 0.0f));
}

Camera::Camera(Camera::projection mode, float width, float height, vec3 coorFrom, vec3 coorTo)
{
  init(mode, width, height, coorFrom, coorTo);
}

void Camera::init(Camera::projection mode, float width, float height, vec3 coorFrom, vec3 coorTo)
{
  d_coorFrom = coorFrom;
  d_coorTo = coorTo;

  d_height = height;
  d_width = width;

  d_fov = 60;
  d_zNear = 0.5;
  d_zFar = 1000;

  d_mode = mode;

  

  setView();

  setProjection();

}

void Camera::setView()
{
  vec2 dir = vec2(d_coorTo.x - d_coorFrom.x, d_coorTo.z - d_coorFrom.z);
  vec2 line_dir1 = rotate(dir, -d_fov);
  vec2 line_dir2 = rotate(dir, d_fov);

  A1 = line_dir1.y;
  A2 = line_dir2.y;
  B1 = -line_dir1.x;
  B2 = -line_dir2.x;
  D1 = -(A1 * d_coorFrom.x + B1 * d_coorFrom.z);
  D2 = -(A2 * d_coorFrom.x + B2 * d_coorFrom.z);

  if(d_mode == Camera::perspective || d_mode == Camera::orthogonal)
  {
    d_view = lookAt(d_coorFrom, d_coorTo, vec3(0.0, 1.0, 0.0));
  }
  else
  {
    d_view = mat4(1.0);
  }

  d_changed = false;
}

void Camera::setProjection()
{
  if(d_mode == Camera::perspective)
  {
  	if(d_height == 0)
  		d_height = 1;

    d_projection = glm::perspective(d_fov, d_width / d_height, d_zNear, d_zFar);

    if(d_height == 1)
    	d_height = 0;
  }
  else if(d_mode == Camera::orthogonal)
  {
    d_projection = ortho(-(d_width/2), d_width/2, -(d_height/2), d_height/2, d_zNear, d_zFar);
  }
  else
  {
    d_projection = ortho(0.0f, d_width, d_height, 0.0f);
  }
}

//void camera::setUp(vec3 up)
//{
//  d_up = up;
//  d_changed = true;
//}

float Camera::fov() const
{
  return d_fov;
}

float Camera::height() const
{
  return d_height;
}

float Camera::width() const
{
  return d_width;
}

mat4 const &Camera::in_mat_view() const
{
	return d_view;
}

mat4 const &Camera::in_mat_projection() const
{
	return d_projection;
}

void Camera::setCoorFrom(vec3 coorFrom)
{
  d_coorFrom = coorFrom;
  d_changed = true;
}

void Camera::setCoorTo(vec3 coorTo)
{
  d_coorTo = coorTo;
  d_changed = true;
}

void Camera::setCoorTo(float dir, float yDir)
{
  d_coorTo = vec3(sin(dir) * cos(yDir),
  		sin(yDir),
  		cos(dir) * cos(yDir));
  d_coorTo += d_coorFrom;
  d_changed = true;
}

void Camera::setFOV(float fov)
{
  d_fov = fov;
  setProjection();
}

void Camera::setZrange(float zNear, float zFar)
{
  d_zNear = zNear;
  d_zFar = zFar;
  setProjection();
}

void Camera::setAtShader(Shader const &shader)
{
  if(d_changed == true)
    setView();

  shader.set("in_mat_view", d_view);
  shader.set("in_mat_projection", d_projection);
}

bool Camera::frustum(float ox, float oz)
{
	return A1 * ox + B1 * oz + D1 < 0 && A2 * ox + B2 * oz + D2 > 0;
}

}
