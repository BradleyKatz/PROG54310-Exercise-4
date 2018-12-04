#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

namespace Graphics
{
	class Shader
	{
	public:
		Shader();
		Shader(Shader&& shader) noexcept;
		Shader(const Shader& shader) = delete;
		~Shader();

		Shader& operator=(const Shader& shader) = delete;
		Shader& operator=(Shader&& shader) noexcept;

		void Compile(const char* vs_path, const char* fs_path);
		void Compile(const char *vs_path, const char *ts_path, const char *gs_path, const char *fs_path);
		void Bind() const;
		static void Unbind();
		
		void SetMatrix4f(const char* name, const glm::mat4& mat) const;
		void SetBool(const char *name, GLboolean value);
		void SetInt(const char *name, GLint value);
		void SetFloat(const char *name, GLfloat value);
		void SetVec3(const char *name, glm::vec3 &vec);
		
	protected:
		static GLuint CompileShader(const char* vs_path, const char* fs_path);
		static GLuint CompileShader(const char *vs_path, const char *ts_path, const char *gs_path, const char *fs_path);
	
	private:
		bool m_compiled;
		unsigned int m_program;
	};
}

