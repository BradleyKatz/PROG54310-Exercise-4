#pragma once
#include <map>
#include "Mesh.h"
#include "Shader.h"
#include "PointLight.h"
#include "Skybox.h"

namespace Graphics
{
	class Scene
	{
	private:
		Scene();

		Skybox *skybox;
		std::vector<Mesh*> meshes;
		std::vector<PointLight*> lights;
	public:
		~Scene();

		void SetAllMeshShaders(Shader *shader);
		void SetAllLightShaders(Shader *shader);
		void SetSkyboxShader(Shader *shader);

		void SetMeshShader(const std::string &meshName, Shader *shader);
		void SetLightShader(const std::string &lightName, Shader *shader);
		void Draw() const;
		static Scene* LoadScene(const char *filepath);
	};
}