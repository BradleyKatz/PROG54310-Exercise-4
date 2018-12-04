#include "PointLight.h"
#include <string>

namespace
{
	glm::vec3 gPointLightAmbient{ 0.05f, 0.05f, 0.05f };
	glm::vec3 gPointLightDiffuse{ 0.8f, 0.8f, 0.8f };
	glm::vec3 gPointLightSpecular{ 1.0f, 1.0f, 1.0f };
	GLfloat gPointLightConstant{ 1.0f };
	GLfloat gPointLightLinear{ 0.09f };
	GLfloat gPointLightQuadratic{ 0.032f };
}

namespace Graphics
{
	GLuint PointLight::nextID = 0;

	PointLight::PointLight() : vao{ 0 }, vbo{ 0 }, id{ nextID++ }, shader{ nullptr },
		name{ "" }, position{ }, vertices{ }
	{
	}

	PointLight::~PointLight()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void PointLight::InitLightData()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0);
	}

	void PointLight::SetShader(Shader *shader)
	{
		this->shader = shader;
	}

	void PointLight::Draw()
	{
		if (shader != nullptr)
		{
			shader->Bind();

			shader->SetVec3((std::string("pointLights[") + std::to_string(id) + std::string("].position")).c_str(), position);
			shader->SetVec3((std::string("pointLights[") + std::to_string(id) + std::string("].ambient")).c_str(), gPointLightAmbient);
			shader->SetVec3((std::string("pointLights[") + std::to_string(id) + std::string("].diffuse")).c_str(), gPointLightDiffuse);
			shader->SetVec3((std::string("pointLights[") + std::to_string(id) + std::string("].specular")).c_str(), gPointLightSpecular);
			shader->SetFloat((std::string("pointLights[") + std::to_string(id) + std::string("].constant")).c_str(), gPointLightConstant);
			shader->SetFloat((std::string("pointLights[") + std::to_string(id) + std::string("].linear")).c_str(), gPointLightLinear);
			shader->SetFloat((std::string("pointLights[") + std::to_string(id) + std::string("].quadratic")).c_str(), gPointLightQuadratic);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			shader->Unbind();
		}
	}
}