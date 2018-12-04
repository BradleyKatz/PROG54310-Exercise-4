#version 330 core
layout (location = 0) in vec3 position;

out vec3 skyboxUV;

uniform mat4 matProjection;
uniform mat4 matView;

void main()
{
	skyboxUV = position;

	vec4 pos = matProjection * mat4(mat3(matView)) * vec4(position, 1.0f); 
	gl_Position = pos.xyww;
}