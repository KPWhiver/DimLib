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

#include "DIM/window.hpp"
#include "DIM/light.hpp"
#include "DIM/shader.hpp"
#include "DIM/camera.hpp"

#include "GL/gtc/matrix_transform.hpp"
#include "GL/gtc/matrix_inverse.hpp"

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

	Shader::active().send(mat4(1.0), "in_mat_model");
	d_in_mat_light = bias * cam.in_mat_projection() * cam.in_mat_view();// * lightprojection * lightview;

	cam.send();
}

void Light::send() const
{
	if(d_mode == Light::directional)
	{
		Shader::active().send(d_lightColor, "in_light[0].lightColor");
		Shader::active().send(d_highlightColor, "in_light[0].highlightColor");
		Shader::active().send(d_lightIntensity, "in_light[0].lightIntensity");
		Shader::active().send(d_ambientIntensity, "in_light[0].ambientIntensity");
		Shader::active().send(d_transformedPosition, "in_light[0].position");
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
		d_transformedPosition = vec4(Shader::active().in_mat_normal() * vec3(d_position), 1.0);
	}
	else
	{
		d_transformedPosition = Shader::active().in_mat_model() * d_position;
	}
}

}
