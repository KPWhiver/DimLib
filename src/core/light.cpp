// light.cpp
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

#include <iostream>

#include "dim/core/light.hpp"
#include "dim/core/shader.hpp"
#include "dim/core/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

using namespace glm;
using namespace std;

namespace dim
{

Light::Light()
:
	d_ambientIntensity(0.0),
	d_lightIntensity(0.0),
	d_highlightColor(0.0),
	d_lightColor(0.0),
	d_position(0.0),
	d_transformedPosition(0.0),
	d_mode(Light::directional)

{
}

Light::Light(vec4 const &position, vec4 const &lightColor, vec4 const &highlightColor, vec4 const &ambientIntensity, vec4 const &lightIntensity, Light::type mode)
:
	d_ambientIntensity(ambientIntensity),
	d_lightIntensity(lightIntensity),
	d_highlightColor(highlightColor),
	d_lightColor(lightColor),
	d_position(position),
	d_transformedPosition(position),
	d_mode(mode)
{
}

mat4 const &Light::lightMatrix() const
{
	return d_lightMatrix;
}

vec4 const &Light::position() const
{
	return d_position;
}

void Light::setupShadowRender(vec3 const &origin, float dir)
{
  //TODO fix shadermess
	vec3 from;
	from.x = origin.x + ((100.0f / 2) * sin(dir));
	from.y = origin.y;
	from.z = origin.z + ((100.0f / 2) * cos(dir));
	//
	Camera cam(Camera::orthogonal, 100, 100, from + vec3(d_position) * -100.0f, from);
	cam.setZrange(50, 200);

	calcLightMatrix(cam);

  cam.setAtShader();
}

void Light::calcLightMatrix(Camera const &cam)
{
	mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
									 0.0, 0.5, 0.0, 0.0,
									 0.0, 0.0, 0.5, 0.0,
									 0.5, 0.5, 0.5, 1.0);

	d_lightMatrix = cam.projectionMatrix() * cam.viewMatrix();
}

void Light::setAtShader() const
{
	if(d_mode == Light::directional)
	{
    Shader::set("in_light[0].lightColor", d_lightColor);
    Shader::set("in_light[0].highlightColor", d_highlightColor);
    Shader::set("in_light[0].lightIntensity", d_lightIntensity);
    Shader::set("in_light[0].ambientIntensity", d_ambientIntensity);
    Shader::set("in_light[0].position", d_transformedPosition);

		if(d_lightMatrix != mat4{})
      Shader::set("in_light[0].lightMatrix", d_lightMatrix);
	}
}

void Light::setAmbientIntensity(vec4 const &ambientIntensity)
{
	d_ambientIntensity = ambientIntensity;
}

void Light::setLightIntensity(vec4 const &lightIntensity)
{
	d_lightIntensity = lightIntensity;
}

void Light::setPosition(vec4 const &position)
{
	d_position = position;
}

/* Transforms the light to camera space */
void Light::transform()
{
// TODO fix
	/*if(d_mode == Light::directional)
	{
		d_transformedPosition = vec4(Shader::active().normalMatrix() * vec3(d_position), 1.0);
	}
	else
	{
		d_transformedPosition = Shader::active().modelMatrix() * d_position;
	}*/
}

}
