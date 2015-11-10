/*
Title: Normal Mapping
File Name: VertexShader.glsl
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
 
layout(location = 0) in vec3 in_position;	// Our position in a vector3 specifying x, y, and z.
layout(location = 1) in vec2 in_tex;		// Our texture coordinates in a vector2 specifying u and v.
layout(location = 2) in vec3 in_normal;		// Normal at the point in the surface.
layout(location = 3) in vec3 in_tangent;	// Surface tangent 

out vec2 texCoord;			// We pass texture coordinates to the fragment shader, where they will actually be used.
out vec3 Position;			// The position of the vertex in model space.
out mat3 toTangentSpace;	// the matrix that converts normal from tangent space to world space.

uniform mat4 trans; // Our uniform transformation matrix.
uniform mat4 MVP;	// Our uniform ModelxViewxProjeciton Matrix

void main(void)
{
	texCoord = in_tex;	// Copy the texture values over.
	Position = in_position;

	//Get the normal in the world space coorinate system
	vec3 normal = normalize(trans * vec4(in_normal,1.0f)).xyz;		
	vec3 tangent = normalize(trans * vec4(in_tangent,1.0f)).xyz;		
	vec3 binormal = normalize(cross(normal, tangent));					// binormal, the vector perpendicular to both normal and the tangent. 
	
	toTangentSpace = mat3(
	tangent.x,binormal.x,normal.x,
	tangent.y,binormal.y,normal.y,
	tangent.z,binormal.z,normal.z);
	
	gl_Position = MVP * vec4(in_position, 1.0); // Pass out the vertices to the gl_Position after transforming them by our matrix.
}