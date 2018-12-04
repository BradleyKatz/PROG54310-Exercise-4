#include "Skybox.h"

namespace
{
	const GLfloat skyboxVertices[] = 
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
}

namespace Graphics
{
	Skybox::Skybox():
		vao{ 0 }, vbo{ 0 }, shader{ nullptr }, texture{ nullptr }
	{
	}

	Skybox::~Skybox()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void Skybox::InitSkyboxData()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

		Texture *texSkybox = Texture::GetTexture("Skybox");
		if (texSkybox)
		{
			texture = texSkybox;
		}
	}

	void Skybox::SetShader(Shader *shader)
	{
		this->shader = shader;
	}

	void Skybox::Draw()
	{
		glDepthFunc(GL_LEQUAL);

		if (shader != nullptr)
		{
			shader->Bind();

			if (texture != nullptr)
			{
				texture->BindTexture();
			}
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (shader != nullptr)
		{
			shader->Unbind();

			if (texture != nullptr)
			{
				texture->UnbindTexture();
			}
		}

		glDepthFunc(GL_LESS);
	}
}