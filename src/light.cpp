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

#include "dim/light.hpp"
#include "dim/shader.hpp"
#include "dim/camera.hpp"

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
	d_in_mat_light(1.0),
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
	d_in_mat_light(1.0),
	d_mode(mode)
{
}

mat4 &Light::in_mat_light()
{
	return d_in_mat_light;
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

	mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
									 0.0, 0.5, 0.0, 0.0,
									 0.0, 0.0, 0.5, 0.0,
									 0.5, 0.5, 0.5, 1.0);

	//Shader::active().send("in_mat_model", mat4(1.0));
	d_in_mat_light = bias * cam.projectionMatrix() * cam.viewMatrix();// * lightprojection * lightview;

	cam.setAtShader(Shader::active());
}

void Light::setAtShader(Shader const &shader) const
{
	if(d_mode == Light::directional)
	{
		shader.set("in_light[0].lightColor", d_lightColor);
		shader.set("in_light[0].highlightColor", d_highlightColor);
		shader.set("in_light[0].lightIntensity", d_lightIntensity);
		shader.set("in_light[0].ambientIntensity", d_ambientIntensity);
		shader.set("in_light[0].position", d_transformedPosition);
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

	if(d_mode == Light::directional)
	{
		d_transformedPosition = vec4(Shader::active().normalMatrix() * vec3(d_position), 1.0);
	}
	else
	{
		d_transformedPosition = Shader::active().modelMatrix() * d_position;
	}
}

}
