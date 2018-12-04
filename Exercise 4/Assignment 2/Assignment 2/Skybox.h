#pragma once
#include <GL\glew.h>
#include "Shader.h"
#include <vector>
#include "Vertex.h"
#include "Texture.h"

namespace Graphics
{
	class Skybox
	{
	private:
		GLuint vao, vbo;
		Shader *shader;
		Texture *texture;

	public:
		Skybox();
		~Skybox();

		void InitSkyboxData();
		void SetShader(Shader *shader);
		void Draw();
	};
}