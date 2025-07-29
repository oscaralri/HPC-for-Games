#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture;

vec3 color = vec3(1.f, 1.f, 1.f);

void main()
{
	vec4 texColor = texture(texture, TexCoord);
    FragColor = vec4(texColor.rgb * color, texColor.a);
}