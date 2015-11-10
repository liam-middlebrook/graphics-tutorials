#version 440

in vec4 position_age;
in vec3 color;

layout (std140) uniform camera
{
	mat4 viewMat;
	mat4 projMat;
	vec4 camPos;
};

out vec4 Position_Age;
out vec3 Color;

void main()
{
	gl_Position = projMat * viewMat * vec4(position_age.xyz, 1.0);
	Position_Age = position_age;
	Color = color;
}