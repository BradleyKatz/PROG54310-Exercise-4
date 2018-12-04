#include "Texture.h"
#include <string>
#include <stb_image\stb_image.h>

namespace Graphics
{
	std::map<std::string, Texture*> Texture::textures;

	Texture::Texture() :
		textureID{ 0 }
	{
		glGenTextures(1, &textureID);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &textureID);
	}

	void Texture::BindTexture() const
	{
		glBindTexture(textureType, textureID);
	}

	void Texture::UnbindTexture() const
	{
		glBindTexture(textureType, NULL);
	}

	void Texture::LoadTexture(const char *filepath)
	{
		Texture *texture = new Texture();
		texture->textureType = GL_TEXTURE_2D;

		// Load the texture from the relative filepath
		{
			GLint width, height, nrChannels;
			GLubyte *data = stbi_load(filepath, &width, &height, &nrChannels, 0); // GLubyte = OpenGL unsigned char

			if (data)
			{
				glBindTexture(GL_TEXTURE_2D, texture->textureID);
				
				GLenum colorFormat;
				switch (nrChannels)
				{
				case 1:
					colorFormat = GL_RED;
					break;
				case 3:
					colorFormat = GL_RGB;
					break;
				case 4:
					colorFormat = GL_RGBA;
					break;
				}

				glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				textures.emplace(std::string(filepath), texture);
			}
			else
			{
				delete texture;
				throw std::exception("Error: Failed to load texture for material");
			}

			stbi_image_free(data);
		}
	}

	void Texture::LoadSkyboxTextures(std::vector<std::string> texFilepaths)
	{
		Texture *currentTexture = new Texture();
		currentTexture->textureType = GL_TEXTURE_CUBE_MAP;

		for (GLuint i = 0; i < texFilepaths.size(); ++i)
		{	
			GLint width, height, nrChannels;
			GLubyte *data = stbi_load(texFilepaths[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum colorFormat;
				switch (nrChannels)
				{
				case 1:
					colorFormat = GL_RED;
					break;
				case 3:
					colorFormat = GL_RGB;
					break;
				case 4:
					colorFormat = GL_RGBA;
					break;
				}

				glBindTexture(GL_TEXTURE_CUBE_MAP, currentTexture->textureID);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				delete currentTexture;
				throw std::exception("Error: Failed to load skybox");
			}

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		if (currentTexture)
		{
			textures.emplace("Skybox", currentTexture);
		}
	}

	Texture* Texture::GetTexture(const std::string &name)
	{
		auto itKey = textures.find(name);

		if (itKey != textures.end())
		{
			return itKey->second;
		}
		else
		{
			return nullptr;
		}
	}
}