#version 440

layout(points) in;

layout(triangle_strip, max_vertices = 4) out;

in vec4 Position_Age[];
in vec3 Color[];

out vec2 TexCoord;
out vec3 FragColor;

void main()
{
	if(Position_Age[0].w >= 0.0)
	{
		gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.0, 0.0);
		TexCoord = vec2(0.0, 0.0);
		FragColor = Color[0];
		EmitVertex();

		gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, 0.0, 0.0);
		TexCoord = vec2(1.0, 0.0);
		FragColor = Color[0];
		EmitVertex();

		gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
		TexCoord = vec2(0.0, 1.0);
		FragColor = Color[0];
		EmitVertex();

		gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
		TexCoord = vec2(1.0, 1.0);
		FragColor = Color[0];
		EmitVertex();

		EndPrimitive();
	}
}