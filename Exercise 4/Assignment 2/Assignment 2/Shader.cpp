#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>

// File-private helper functions
namespace
{
	void GetShaderSource(const char *shaderFilePath, std::string &strSource)
	{
		std::ifstream shaderStream(shaderFilePath, std::ios::in);
		if (shaderStream.is_open())
		{
			std::string currentLine = "";

			while (std::getline(shaderStream, currentLine))
				strSource += ("\n" + currentLine);

			shaderStream.close();
		}
		else
		{
			std::cerr << "Failed to open shader file: " << shaderFilePath;
		}
	}
}

namespace Graphics
{
	Shader::Shader() : m_compiled(false), m_program(0) { }

	Shader::~Shader()
	{
		if (m_compiled)
		{
			glDeleteProgram(m_program);
		}
	}

	void Graphics::Shader::Compile(const char *vsPath, const char *fsPath)
	{
		m_compiled = true;
		m_program = CompileShader(vsPath, fsPath);
	}

	void Graphics::Shader::Compile(const char *vs_path, const char *ts_path, const char *gs_path, const char *fs_path)
	{
		m_compiled = true;
		m_program = CompileShader(vs_path, ts_path, gs_path, fs_path);
	}

	void Graphics::Shader::Bind() const
	{
		if (m_compiled)
		{
			glUseProgram(m_program);
		}
	}

	void Graphics::Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::SetMatrix4f(const char *name, const glm::mat4 &mat) const
	{
		GLint id = glGetUniformLocation(m_program, name);
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void Shader::SetBool(const char *name, GLboolean value)
	{
		GLint id = glGetUniformLocation(m_program, name);
		glUniform1i(id, static_cast<int>(value));
	}

	void Shader::SetInt(const char *name, GLint value)
	{
		GLint id = glGetUniformLocation(m_program, name);
		glUniform1i(id, value);
	}

	void Shader::SetFloat(const char *name, GLfloat value)
	{
		GLint id = glGetUniformLocation(m_program, name);
		glUniform1f(id, value);
	}

	void Shader::SetVec3(const char *name, glm::vec3 &vec)
	{
		GLint id = glGetUniformLocation(m_program, name);
		glUniform3fv(id, 1, glm::value_ptr(vec));
	}

	Shader::Shader(Shader &&shader) noexcept : m_compiled(shader.m_compiled), m_program(shader.m_program) { }

	Shader& Shader::operator=(Shader&& shader) noexcept
	{
		if (this == &shader)
		{
			return *this;
		}

		m_compiled = shader.m_compiled;
		m_program = shader.m_program;

		return *this;
	}

	GLuint Graphics::Shader::CompileShader(const char *vsPath, const char *fsPath)
	{
		GLuint vsShaderId;
		GLuint fsShaderId;
		GLuint program;

		int logLength;

		std::string vsSource;
		GetShaderSource(vsPath, vsSource);

		std::string fsSource;
		GetShaderSource(fsPath, fsSource);

		const char* ccVs = vsSource.c_str();
		const char* ccFs = fsSource.c_str();

		// compile vertex shader

		vsShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsShaderId, 1, &ccVs, nullptr);
		glCompileShader(vsShaderId);

		// log any vs error
		
		// grab the length of the error message
		glGetShaderiv(vsShaderId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength != 0)
		{
			std::cout << "Vertex shader error: ";
			char* vertLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength))); // reserve the space upfront for the error message
			glGetShaderInfoLog(vsShaderId, logLength, nullptr, vertLogMessage);
			std::cout << vertLogMessage << std::endl;
			delete vertLogMessage;
		}
		else
		{
			std::cout << "Vertex Shader Compiled Successfully" << std::endl;
		}

		// compile fragment shader

		fsShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsShaderId, 1, &ccFs, nullptr);
		glCompileShader(fsShaderId);

		// grab length of the error message
		glGetShaderiv(fsShaderId, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength != 0)
		{
			std::cout << "Fragment shader error: ";
			char* fragLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength))); // reserve the space upfront for the error message
			glGetShaderInfoLog(fsShaderId, logLength, nullptr, fragLogMessage);
			std::cout << fragLogMessage << std::endl;
			delete fragLogMessage;
		}
		else
		{
			std::cout << "Fragment Shader Compiled Successfully" << std::endl;
		}

		program = glCreateProgram();
		glAttachShader(program, vsShaderId);
		glAttachShader(program, fsShaderId);
		glLinkProgram(program);

		// Check program linking status
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

		if (isLinked == GL_FALSE)
		{
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				std::cout << "Shader program linking error: ";
				char *linkLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
				glGetProgramInfoLog(program, logLength, nullptr, linkLogMessage);
				std::cout << linkLogMessage << std::endl;
				delete linkLogMessage;
			}
		}

		// opengl has linked to program so we can delete the copy
		glDeleteShader(vsShaderId);
		glDeleteShader(fsShaderId);

		return program;
	}

	GLuint Shader::CompileShader(const char *vs_path, const char *ts_path, const char *gs_path, const char *fs_path)
	{
		GLuint vsShaderId, tsShaderId, gsShaderId, fsShaderId;
		GLuint program;

		int logLength;

		std::string vsSource;
		GetShaderSource(vs_path, vsSource);

		std::string tsSource;
		GetShaderSource(ts_path, tsSource);

		std::string gsSource;
		GetShaderSource(gs_path, gsSource);

		std::string fsSource;
		GetShaderSource(fs_path, fsSource);

		const char *ccVs = vsSource.c_str();
		const char *ccTs = tsSource.c_str();
		const char *ccGs = gsSource.c_str();
		const char *ccFs = fsSource.c_str();

		// Compile vertex shader
		{
			vsShaderId = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vsShaderId, 1, &ccVs, nullptr);
			glCompileShader(vsShaderId);

			glGetShaderiv(vsShaderId, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength != 0)
			{
				std::cout << "Vertex shader error: ";
				char* vertLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength))); // reserve the space upfront for the error message
				glGetShaderInfoLog(vsShaderId, logLength, nullptr, vertLogMessage);
				std::cout << vertLogMessage << std::endl;
				delete vertLogMessage;
			}
			else
			{
				std::cout << "Vertex Shader Compiled Successfully" << std::endl;
			}
		}

		// Compile tessellation shader
		{
			tsShaderId = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tsShaderId, 1, &ccTs, nullptr);
			glCompileShader(tsShaderId);

			glGetShaderiv(tsShaderId, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength != 0)
			{
				std::cout << "Tessellation shader error: ";
				char *tessLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
				glGetShaderInfoLog(tsShaderId, logLength, nullptr, tessLogMessage);
				std::cout << tessLogMessage << std::endl;
				delete tessLogMessage;
			}
			else
			{
				std::cout << "Tessellation Shader Compiled Successfully" << std::endl;
			}
		}

		// Compile geometry shader
		{
			gsShaderId = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(gsShaderId, 1, &ccGs, nullptr);
			glCompileShader(gsShaderId);

			glGetShaderiv(gsShaderId, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength != 0)
			{
				std::cout << "Geometry shader error: ";
				char *geomLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
				glGetShaderInfoLog(gsShaderId, logLength, nullptr, geomLogMessage);
				std::cout << geomLogMessage << std::endl;
				delete geomLogMessage;
			}
			else
			{
				std::cout << "Geometry Shader Compiled Successfully" << std::endl;
			}
		}

		// Compile fragment shader
		{
			fsShaderId = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fsShaderId, 1, &ccFs, nullptr);
			glCompileShader(fsShaderId);

			// grab length of the error message
			glGetShaderiv(fsShaderId, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength != 0)
			{
				std::cout << "Fragment shader error: ";
				char* fragLogMessage = static_cast<char*>(malloc(static_cast<size_t>(logLength))); // reserve the space upfront for the error message
				glGetShaderInfoLog(fsShaderId, logLength, nullptr, fragLogMessage);
				std::cout << fragLogMessage << std::endl;
				delete fragLogMessage;
			}
			else
			{
				std::cout << "Fragment Shader Compiled Successfully" << std::endl;
			}
		}

		program = glCreateProgram();
		glAttachShader(program, vsShaderId);
		glAttachShader(program, tsShaderId);
		glAttachShader(program, gsShaderId);
		glAttachShader(program, fsShaderId);
		glLinkProgram(program);

		// opengl has linked to program so we can delete the copy
		glDeleteProgram(vsShaderId);
		glDeleteProgram(tsShaderId);
		glDeleteProgram(gsShaderId);
		glDeleteProgram(fsShaderId);

		return program;
	}
}
