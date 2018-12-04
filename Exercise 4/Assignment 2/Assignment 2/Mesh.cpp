#include "Mesh.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <glm\gtc\matrix_transform.hpp>

namespace Graphics
{
	Mesh::Mesh() : vao(0u), vbo(0u), ebo(0u), shader(nullptr), material(nullptr) 
	{ 
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		delete material;
	}

	const std::string& Mesh::GetName() { return name; }

	// Sends the mesh data to OpenGL so that we only have to bind the mesh's buffers to draw it later, reduces overhead
	void Mesh::InitMeshData()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

		glBindVertexArray(0);
	}

	void Mesh::Draw() const
	{
		if (shader != nullptr)
		{
			shader->Bind();

			if (material != nullptr)
			{
				if (material->texDiffuse != nullptr)
				{
					glActiveTexture(GL_TEXTURE0);
					material->texDiffuse->BindTexture();
				}

				if (material->texSpecular != nullptr)
				{
					glActiveTexture(GL_TEXTURE1);
					material->texSpecular->BindTexture();
				}

				shader->SetVec3("material.ka", material->ka);
				shader->SetVec3("material.kd", material->kd);
				shader->SetVec3("material.ks", material->ks);
				shader->SetFloat("material.alpha", material->alpha);
			};

			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);

			if (material != nullptr) 
			{ 
				if (material->texDiffuse != nullptr)
				{
					glActiveTexture(GL_TEXTURE0);
					material->texDiffuse->UnbindTexture();
				}

				if (material->texSpecular != nullptr)
				{
					glActiveTexture(GL_TEXTURE1);
					material->texSpecular->UnbindTexture();
				}
			};

			shader->Unbind();
		}
	}

	void Mesh::SetShader(Shader *shader)
	{
		this->shader = shader;
	}

	Mesh* Mesh::LoadMesh(std::ifstream &fileStream, GLuint faceIndexOffset)
	{
		Mesh *mesh = new Mesh();

		// Load the mesh vertices, normals, etc.
		{
			if (fileStream.is_open())
			{
				GLboolean isEndOfCurrentMesh = false;
				std::string currentLine;

				std::vector<glm::vec2> tempUVs;
				std::vector<glm::vec3> tempNormals;

				GLuint vertexIndex[3], uvIndex[3], normalIndex[3];

				std::vector<GLuint> vertexIndices, uvIndices, normalIndices;

				// For parsing vertices and whatnot out of each line
				std::istringstream istream;

				while (!isEndOfCurrentMesh)
				{
					std::getline(fileStream, currentLine);

					if (std::strncmp(currentLine.c_str(), "vn", 2) == 0)
					{
						// Process the next normal
						glm::vec3 currentNormal;

						istream.str(std::string(currentLine.c_str() + 3));
						istream >> currentNormal.x;
						istream >> currentNormal.y;
						istream >> currentNormal.z;
						istream.clear();

						tempNormals.push_back(currentNormal);
					}
					else if (std::strncmp(currentLine.c_str(), "vt", 2) == 0)
					{
						// Process the next texture coordinate
						glm::vec2 currentUV;

						istream.str(std::string(currentLine.c_str() + 3));
						istream >> currentUV.x;
						istream >> currentUV.y;
						istream.clear();

						tempUVs.push_back(currentUV);
					}
					else if (std::strncmp(currentLine.c_str(), "v", 1) == 0)
					{
						// Process the next vertex
						Vertex currentVertex;

						istream.str(std::string(currentLine.c_str() + 2));
						istream >> currentVertex.position.x;
						istream >> currentVertex.position.y;
						istream >> currentVertex.position.z;
						istream.clear();

						mesh->vertices.push_back(currentVertex);
					}
					else if (std::strncmp(currentLine.c_str(), "f", 1) == 0)
					{
						// Process the next face
						istream.str(std::string(currentLine.c_str() + 2));

						istream >> vertexIndex[0];
						istream.ignore(1);
						istream >> uvIndex[0];
						istream.ignore(1);
						istream >> normalIndex[0];

						istream >> vertexIndex[1];
						istream.ignore(1);
						istream >> uvIndex[1];
						istream.ignore(1);
						istream >> normalIndex[1];

						istream >> vertexIndex[2];
						istream.ignore(1);
						istream >> uvIndex[2];
						istream.ignore(1);
						istream >> normalIndex[2];

						istream.clear();

						for (GLuint i = 0; i < 3; ++i)
						{
							vertexIndex[i] -= faceIndexOffset;
							uvIndex[i] -= faceIndexOffset;
							normalIndex[i] -= faceIndexOffset;

							// Link the normal and uv coordinates to each previously defined vertex
							mesh->vertices[vertexIndex[i] - 1].normal = tempNormals[normalIndex[i] - 1];
							mesh->vertices[vertexIndex[i] - 1].textureCoords = tempUVs[uvIndex[i] - 1];

							// Register the vertex index in the mesh
							mesh->indices.push_back(vertexIndex[i] - 1);
						}
					}
					else if (std::strncmp(currentLine.c_str(), "usemtl", 6) == 0)
					{
						// Set the texture for the mesh
						Material *material = Material::GetMaterial(std::string(currentLine.c_str() + 7));

						if (material != nullptr)
						{
							mesh->material = material;
						}
					}

					// Check to see if we've reached the end of the current face
					GLchar nextChar = fileStream.peek();
					if (nextChar == 'g' || nextChar == EOF)
					{
						isEndOfCurrentMesh = true;
					}
				}
			}
			else
			{
				throw "Error: Failed to open mesh file";
			}
		}

		// Calculate the mesh's position vector (Average of all vertices)
		glm::vec3 average;
		for (const Vertex &vertex : mesh->vertices)
		{
			average += vertex.position;
		}

		average /= mesh->vertices.size();
		mesh->transform.position = average;

		// Send the mesh data to the GPU before returning
		mesh->InitMeshData();
		return mesh;
	}
}