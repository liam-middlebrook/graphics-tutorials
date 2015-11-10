#version 440

in vec3 position;
in vec2 texCoord;
in vec3 normal;

layout (std140) uniform camera
{
	mat4 viewMat;
	mat4 projMat;
	vec4 camPos;
};

layout (std140) uniform perModel
{
	mat4 modelMat;
	mat4 normalTransformMat;
	vec4 color;
};

out vertToFrag
{
	vec4 Color;
	vec4 Normal;
	vec4 WorldPos;
	vec4 CamPos;
	vec4 ViewVec;
	vec2 TexCoord;
};

void main()
{
	Color = color;
	Normal =  normalTransformMat * vec4(normal.xyz, 0.0);
	WorldPos = modelMat * vec4(position.xyz, 1.0);
	CamPos = camPos;
	gl_Position = projMat * viewMat * modelMat * vec4(position.xyz, 1.0);
	TexCoord = texCoord;
}