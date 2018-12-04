#pragma once

#include <GL\glew.h>
#include <map>
#include <vector>

namespace Graphics
{
	class Texture
	{
	private:
		static std::map<std::string, Texture*> textures;

		GLuint textureID;
		GLenum textureType;

		Texture();

		static void LoadTexture(const char *filepath);
		static void LoadSkyboxTextures(std::vector<std::string> texFilepaths);

	public:
		~Texture();

		static Texture* GetTexture(const std::string &name);

		void BindTexture() const;
		void UnbindTexture() const;

		friend class Material;
		friend class Mesh;
		friend class Scene;
	};
}