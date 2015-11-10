#version 440

in vertToFrag
{
	vec4 Color;
	vec4 Normal;
	vec4 WorldPos;
	vec4 CamPos;
	vec4 ViewVec;
	vec2 TexCoord;
};

out vec4 outColor;

uniform sampler2D tex;

void main()
{
	outColor = texture(tex, TexCoord);
}