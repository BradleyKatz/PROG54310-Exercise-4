#include <stb_image/stb_image.h>
#include "Material.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

std::map<std::string, Graphics::Material*> Graphics::Material::materials;

Graphics::Material::Material(): 
	texDiffuse{ nullptr }, texSpecular{ nullptr }, alpha{ 1.0f }, illum{ 0u }, ka{ 1.0f, 1.0f, 1.0f }, kd{ 1.0f, 1.0f, 1.0f },
	ks{ 1.0f, 1.0f, 1.0f }
{
}

Graphics::Material::~Material() 
{ 
}

void Graphics::Material::LoadMaterials(const char *filepath)
{
	std::ifstream fileStream;
	fileStream.open(filepath, std::ios::in);

	if (fileStream.is_open())
	{
		std::string currentLine, currentMatName;
		std::istringstream istream;
		Material *material = nullptr;

		while (!fileStream.eof())
		{
			std::getline(fileStream, currentLine);

			if (std::strncmp(currentLine.c_str(), "newmtl", 6) == 0)
			{
				if (material != nullptr)
				{
					materials[currentMatName] = material;
				}

				istream.str(std::string(currentLine.c_str() + 7));
				istream >> currentMatName;
				istream.clear();

				material = new Material();
			}
			else if (std::strncmp(currentLine.c_str(), "map_Kd", 6) == 0)
			{
				std::string textureFilepath, relativeFilepath;

				textureFilepath = std::string(currentLine.c_str() + 7);

				// textureFilepath is an absolute filepath, trim the texture name from that
				relativeFilepath = std::string(textureFilepath.c_str() + (textureFilepath.rfind("\\") + 1));

				Texture *tex = Texture::GetTexture(relativeFilepath);
				if (tex == nullptr)
				{
					Texture::LoadTexture(relativeFilepath.c_str());

					tex = Texture::GetTexture(relativeFilepath);
					if (tex)
					{
						material->texDiffuse = tex;
						material->texSpecular = tex;
					}
					else
					{
						material->alpha = 0.5f;
					}
				}
				else
				{
					material->texDiffuse = tex;
					material->texSpecular = tex;
				}
			}
			/*else if (std::strncmp(currentLine.c_str(), "map_Ks", 6) == 0)
			{
				std::string textureFilepath, relativeFilepath;
				
				textureFilepath = std::string(currentLine.c_str() + 7);
				relativeFilepath = std::string(textureFilepath.c_str() + (textureFilepath.rfind("\\") + 1));

				Texture *tex = Texture::GetTexture(relativeFilepath);
				if (tex == nullptr)
				{
					Texture::LoadTexture(relativeFilepath.c_str());

					tex = Texture::GetTexture(relativeFilepath);
					if (tex)
					{
						material->texSpecular = tex;
					}
				}
				else
				{
					material->texSpecular = tex;
				}
			}*/
			else if (std::strncmp(currentLine.c_str(), "Ka", 2) == 0)
			{
				istream.str(std::string(currentLine.c_str() + 3));
				istream >> material->ka.x;
				istream >> material->ka.y;
				istream >> material->ka.z;
				istream.clear();
			}
			else if (std::strncmp(currentLine.c_str(), "Kd", 2) == 0)
			{
				istream.str(std::string(currentLine.c_str() + 3));
				istream >> material->kd.x;
				istream >> material->kd.y;
				istream >> material->kd.z;
				istream.clear();
			}
			else if (std::strncmp(currentLine.c_str(), "Ks", 2) == 0)
			{
				istream.str(std::string(currentLine.c_str() + 3));
				istream >> material->ks.x;
				istream >> material->ks.y;
				istream >> material->ks.z;
				istream.clear();
			}
			else if (std::strncmp(currentLine.c_str(), "illum", 5) == 0)
			{
				istream.str(std::string(currentLine.c_str() + 6));
				istream >> material->illum;
				istream.clear();
			}
		}
	}
	else
	{
		throw "Error: Failed to open material library file";
	}
}

Graphics::Material* Graphics::Material::GetMaterial(const std::string &name)
{
	auto itKey = materials.find(name);
	
	if (itKey != materials.end())
		return itKey->second;
	else
		return nullptr;
}