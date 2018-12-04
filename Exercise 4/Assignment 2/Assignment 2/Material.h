#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <map>
#include "Texture.h"

namespace Graphics
{
	class Material
	{
	private:
		static std::map<std::string, Material*> materials;

		Texture *texDiffuse;
		Texture *texSpecular;
		GLfloat alpha;
		GLfloat illum;
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;

		Material();

		static void LoadMaterials(const char *filepath);
		static Material* GetMaterial(const std::string &name);
	public:
		~Material();

		friend class Scene;
		friend class Mesh;
	};
}