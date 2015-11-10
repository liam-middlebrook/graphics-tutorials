/*
Title: Normal Mapping
File Name: FragmentShader.glsl
Copyright © 2015
Original authors: Srinivasan T
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
This program demonstrates the implementation of normal maps to produce finer
lighting details.
Normal maps are also textures, but instead of using the rgb value at a pixel
to select the color, we use the rgb values x,y and z values of the normal at that point.
This allows us to define different normal at for each pixel. We use this normal
to do the lighting calculations. This makes the the object look like it has finer details
than the base geometry.
One complication which arises, is that the normal in the normal map are in tangent space.
The vertices we send to the GPU now also have a 3rd parameter, tangent. This should be same
for all the vertices in the object. Otherwise the lighting calculation will look out of the ordinary.

In this example, We render a plane and map a texture on it and light it using the
normal obtained from the normal map. Instead of passing just one texture to the GPU,
we send two: the texture itself and its normal map. Inside the vertexShader, we compute
a matrix using the normal, tangent and binormal to convert the point from model space to
tangent space. This matrix is passed to the fragment shader.
In fragment shader the normal is extracted from the normal map texture and multiplied with the
toTangentSpace matrix. This gives us the normal in world space coordinate system.

Using this normal, we do the lighting calculations and display the color.
In this example, we do not pass any lighting data to teh GPU. The light is a constant point at (3,3,3)
and it is created in the GPU itself.
For more on Lighting consider looking at other examples such as : Directional Light, Point Lights,
Spot Lights , etc.

References:
OpenGL 4 Shading Language Cookbook, Second edition by David Wolff.
*/

#version 430 core // Identifies the version of the shader, this line must be on a separate line from the rest of the shader code

in vec2 texCoord; 	// Our texture coordinates in a vector2 specifying u and v.
in vec3 Position;	// Position of the point in model space.
in mat3 toTangentSpace;

layout(location = 0) out vec4 out_color; // Establishes the variable we will pass out of this shader.

layout(binding = 0) uniform sampler2D tex;				// A sampler that will sample each pixel of the texture.
layout(binding = 1) uniform sampler2D norm;				// A sampler that will sample each pixel of the normal map.

// This is our point light. This data is usually sent in by the openGl application.
// For our example, we used a constant value which we difine here, inside the shader.
struct pointLight{
	vec3 position ;
	vec3 intensity;			// ADS (ambient Diffuse and Specular) intensity
}pointlight;

void main(void)
{
	pointLight pL;
	pL.position = vec3(3,3,3);
	pL.intensity = vec3(0.3,0.5f,0);
	vec3 normal = texture(norm, texCoord).xyz;

	vec3 lightDir = vec3(3, 0, 0) - Position;
	normal = toTangentSpace * normal;
	
	// A call to texture(sampler, coordinate) is what actually does the sampling of the texture.
	out_color = texture(tex, texCoord)* (clamp(dot(normal, -lightDir)* pL.intensity.y,0,1) + pL.intensity.x);
}