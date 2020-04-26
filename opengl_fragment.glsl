#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 vertexTexCoord;

uniform sampler2D textureData;

void main()
{
//   FragColor = vec4(vertexColor, 1);
	FragColor = texture(textureData, vertexTexCoord) * vec4(vertexColor, 1.0f);
}
