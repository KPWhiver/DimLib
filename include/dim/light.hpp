// light.hpp
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

#ifndef LIGHTS_HPP
#define	LIGHTS_HPP

#include "dim/dim.hpp"

namespace dim
{

class Light
{
  glm::vec4 d_ambientIntensity; /* Later to be replaced with ambient occlusion */
  glm::vec4 d_lightIntensity;
  glm::vec4 d_highlightColor;
  glm::vec4 d_lightColor;
  glm::vec4 d_position;
  glm::vec4 d_transformedPosition;

  glm::mat4 d_in_mat_light;

public:
  enum type
  {
  	directional,
  	point,
  };
  
  Light();
  Light(glm::vec4 const &position, glm::vec4 const &lightColor, glm::vec4 const &highlightColor, glm::vec4 const &ambientIntensity, glm::vec4 const &lightIntensity, Light::type mode);
  void setupShadowRender(glm::vec3 const &origin, float dir);
  void transform();
  
  void setPosition(glm::vec4 const &position);
  void setAmbientIntensity(glm::vec4 const &ambientIntensity);
  void setLightIntensity(glm::vec4 const &lightIntensity);
  
  glm::mat4 &in_mat_light();

  void send() const;
  
private:
  Light::type d_mode;

};
}
#endif	/* LIGHTS_HPP */

