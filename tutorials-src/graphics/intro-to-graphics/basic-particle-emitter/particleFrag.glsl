#version 440

out vec4 outColor;

in vec2 TexCoord;
in vec3 FragColor;

uniform sampler2D tex;

void main()
{
	outColor = texture(tex, TexCoord) * vec4(FragColor, 1.0);
}