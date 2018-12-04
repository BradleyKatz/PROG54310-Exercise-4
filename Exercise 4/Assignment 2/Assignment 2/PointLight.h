#pragma once
#include <GL\glew.h>
#include "Shader.h"
#include <vector>
#include "Vertex.h"

namespace Graphics
{
	class PointLight
	{
	private:
		static GLuint nextID;

		GLuint vao, vbo, id;
		Shader *shader;
		std::string name;
		glm::vec3 position;
		std::vector<glm::vec3> vertices;

	public:
		PointLight();
		~PointLight();

		void InitLightData();
		void SetShader(Shader *shader);
		void Draw();

		friend class Scene;
	};
}