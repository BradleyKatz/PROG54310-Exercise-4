#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

out vec2 UV;
out vec3 Normal;
out vec3 FragPos_WorldSpace;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
	// Invert the UVs so that textures appear upright.
	// The UVs come in backwards from Unity for some reason.
	UV = vec2(vertexUV.s, 1.0 - vertexUV.t);

	FragPos_WorldSpace = vec3(matModel * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(matModel))) * vertexNormal;

	gl_Position = matProjection * matView * matModel * vec4(FragPos_WorldSpace, 1.0f);
}