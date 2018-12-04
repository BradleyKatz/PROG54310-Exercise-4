#include "Scene.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

namespace Graphics
{
	Scene::Scene() : meshes{ } { }
	Scene::~Scene() { }

	void Scene::Draw() const 
	{ 
		// Traverse the list in reverse order since the obj exporter exported the unity scene in reverse order
		for (GLint i = meshes.size() - 1; i > -1; --i) 
		{ 
			meshes[i]->Draw(); 
		} 

		for (GLint i = lights.size() - 1; i > -1; --i)
		{
			lights[i]->Draw();
		}

		if (skybox != nullptr)
		{
			skybox->Draw();
		}
	}

	void Scene::SetAllMeshShaders(Shader *shader)
	{
		if (shader != nullptr)
		{
			for (Mesh *mesh : meshes)
			{
				mesh->SetShader(shader);
			}
		}
	}

	void Scene::SetAllLightShaders(Shader *shader)
	{
		if (shader != nullptr)
		{
			for (PointLight *light : lights)
			{
				light->SetShader(shader);
			}

			shader->Bind();
			shader->SetInt("numPointLights", lights.size());
			shader->Unbind();
		}
	}

	void Scene::SetSkyboxShader(Shader *shader)
	{
		if (shader != nullptr && skybox != nullptr)
		{
			skybox->SetShader(shader);
		}
	}

	void Scene::SetMeshShader(const std::string &meshName, Shader *shader)
	{
		if (shader != nullptr)
		{
			for (Mesh *mesh : meshes)
			{
				if (mesh->name == meshName)
				{
					mesh->SetShader(shader);
					return;
				}
			}
		}
	}

	void Scene::SetLightShader(const std::string &lightName, Shader *shader)
	{
		if (shader != nullptr)
		{
			for (PointLight *light : lights)
			{
				if (light->name == lightName)
				{
					light->SetShader(shader);
				}
			}

			shader->Bind();
			shader->SetInt("numPointLights", lights.size());
			shader->Unbind();
		}
	}

	Scene* Scene::LoadScene(const char *filepath)
	{
		Scene *scene = new Scene();

		// Generate the scene's skybox
		{
			std::vector<std::string> skyboxPaths
			{
				"right.jpg",
				"left.jpg",
				"top.jpg",
				"bottom.jpg",
				"front.jpg",
				"back.jpg"
			};

			Texture::LoadSkyboxTextures(skyboxPaths);

			Skybox *skybox = new Skybox();
			skybox->InitSkyboxData();

			scene->skybox = skybox;
		}

		// Load every mesh from the scene and append it to the list of meshes
		{
			GLuint faceIndexOffset = 0u;

			std::ifstream fileStream;
			fileStream.open(filepath, std::ios::in);

			if (fileStream.is_open())
			{
				std::string currentLine;
				std::istringstream istream;

				while (!fileStream.eof())
				{
					std::getline(fileStream, currentLine);

					if (std::strncmp(currentLine.c_str(), "g", 1) == 0)
					{
						Mesh *currentMesh = Graphics::Mesh::LoadMesh(fileStream, faceIndexOffset);
						faceIndexOffset += currentMesh->vertices.size();

						istream.str(std::string(currentLine.c_str() + 2));
						std::getline(istream, currentMesh->name);

						// Determine if the current mesh is a streetlight and act accordingly
						// Find a better way to do this some other time.
						{
							if (std::strncmp(currentMesh->name.c_str(), "streetlight", 11) == 0)
							{
								PointLight *currentLight = new PointLight();
								currentLight->name = currentMesh->name;
								currentLight->position = currentMesh->transform.position;
								
								for (const Vertex &vertex : currentMesh->vertices)
								{
									currentLight->vertices.push_back(vertex.position);
								}

								currentLight->InitLightData();

								scene->lights.push_back(currentLight);
							}
						}

						istream.clear();

						//if (currentMesh != nullptr) { scene->meshes.emplace(currentMesh->name, currentMesh); }
						if (currentMesh != nullptr) { scene->meshes.push_back(currentMesh); }
					}
					else if (std::strncmp(currentLine.c_str(), "mtllib", 6) == 0)
					{
						// Read the material file and load the material for each mesh
						Material::LoadMaterials(currentLine.c_str() + 7);
					}
				}
			}
			else
			{
				throw "Error: Failed to open scene file";
			}

			fileStream.close();
		}

		return scene;
	}
}