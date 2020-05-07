#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 vertexTexCoord;

uniform sampler2D textureData;
uniform sampler2D textureData2;
uniform float opacity;

void main()
{
	FragColor = mix(texture(textureData, vertexTexCoord), texture(textureData2, vec2(-vertexTexCoord.x, vertexTexCoord.y)), opacity);
}
