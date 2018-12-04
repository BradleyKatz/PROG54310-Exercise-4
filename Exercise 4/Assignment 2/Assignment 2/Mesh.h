#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include "Vertex.h"
#include "Material.h"
#include "Shader.h"
#include "Transform.h"

namespace Graphics
{
	class Mesh
	{
	private:
		Mesh();

		GLuint vao, vbo, ebo;

		void InitMeshData();

	protected:
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		Shader *shader;
		Material *material;
		Transform transform;

		static Mesh* LoadMesh(std::ifstream &fileStream, GLuint faceIndexOffset);
		friend class Scene;

	public:
		~Mesh();

		const std::string& GetName();
		void SetShader(Shader *shader);
		void Draw() const;
	};
}