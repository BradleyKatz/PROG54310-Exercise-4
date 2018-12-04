#include "Window.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h> 
#include "Time.h"

namespace
{
	GLfloat gAmbientLightStrength{ 0.2f };

	glm::vec3 gDirLightDirection{ -0.2f, -1.0f, -0.3f };
	glm::vec3 gDirLightAmbient{ 0.05f, 0.05f, 0.05f };
	glm::vec3 gDirLightDiffuse{ 0.4f, 0.4f, 0.4f };
	glm::vec3 gDirLightSpecular{ 0.5f, 0.5f, 0.5f };

	glm::vec3 gSpotLightAmbient{ 0.05f, 0.05f, 0.05f };
	glm::vec3 gSpotLightDiffuse{ 1.5f, 1.5f, 1.5f };
	glm::vec3 gSpotLightSpecular{ 1.5f, 1.5f, 1.5f };
	GLfloat gSpotLightConstant{ 1.0f };
	GLfloat gSpotLightLinear{ 0.09f };
	GLfloat gSpotLightQuadratic{ 0.032f };
	GLfloat gSpotLightInnerCutoff{ 15.0f };
	GLfloat gSpotLightOuterCutoff{ 20.0f };

	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;
	}
}

namespace Graphics
{
	Window::Window(int width, int height, const std::string & title)
	{
		// initialize glfw
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwSetErrorCallback(Window::error_cb);

		m_width = static_cast<float>(width);
		m_height = static_cast<float>(height);
		m_title = title;
		m_window = std::unique_ptr<GLFWwindow, GLFWDeleter>(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));

		// have the glfwwindow know about custom window class
		glfwSetWindowUserPointer(m_window.get(), this);
		glfwSetKeyCallback(m_window.get(), Window::keyboard_cb);
		glfwSetScrollCallback(m_window.get(), Window::scroll_cb);
		glfwSetCursorPosCallback(m_window.get(), Window::mouse_cb);

		glfwMakeContextCurrent(m_window.get());
		glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// glew initialize
		static bool s_window_init = false;
		if (!s_window_init)
		{
			s_window_init = true;
			glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK)
			{
				std::cout << "GLEW INIT FAILED!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		GLint flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

		m_objShader = new Shader();
		m_objShader->Compile("VertShaderOBJ.vert", "FragShaderOBJ.frag");

		m_skyboxShader = new Shader();
		m_skyboxShader->Compile("VertShaderSkybox.vert", "FragShaderSkybox.frag");

		lastX = m_width / 2.0f;
		lastY = m_height / 2.0f;
		m_camera = new Camera(glm::vec3(1.4f, 3.3f, 29.9f));
		/*m_camera->width = static_cast<GLfloat>(width);
		m_camera->height = static_cast<GLfloat>(height);*/

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	Window::~Window()
	{
	}

	void Window::LoadScene(Scene *scene)
	{
		m_scene = scene;
	}

	void Window::run()
	{
		if (m_objShader != nullptr &&  m_scene != nullptr)
		{
			m_scene->SetAllMeshShaders(m_objShader);
			m_scene->SetAllLightShaders(m_objShader);
			m_scene->SetSkyboxShader(m_skyboxShader);

			do
			{
				glfwPollEvents();
				draw();
			} while (glfwGetKey(m_window.get(), GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_window.get()) == 0);
		}
	}

	void Window::error_cb(int error, const char * description)
	{
		std::cout << "Error (" << error << "): " << description << std::endl;
	}

	void Window::keyboard_cb(GLFWwindow * window, int key, int scancode, int action, int mods)
	{
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->keyPress(key, scancode, action, mods);
	}

	void Window::scroll_cb(GLFWwindow *window, GLdouble xOffset, GLdouble yOffset)
	{
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->OnMouseScroll(xOffset, yOffset);
	}

	void Window::mouse_cb(GLFWwindow *window, GLdouble xPos, GLdouble yPos)
	{
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->OnMouseMove(xPos, yPos);
	}

	void Window::keyPress(int key, int scancode, int action, int mods)
	{
		static GLboolean isWireframeEnabled = false;

		// Player directional movement
		{
			if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				//m_camera->Move(eMovementType::Forwards);
				m_camera->ProcessKeyboard(FORWARD, Time::Instance()->GetDeltaTime());
			}

			if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				//m_camera->Move(eMovementType::Left);
				m_camera->ProcessKeyboard(LEFT, Time::Instance()->GetDeltaTime());
			}

			if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				//m_camera->Move(eMovementType::Backwards);
				m_camera->ProcessKeyboard(BACKWARD, Time::Instance()->GetDeltaTime());
			}

			if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				//m_camera->Move(eMovementType::Right);
				m_camera->ProcessKeyboard(RIGHT, Time::Instance()->GetDeltaTime());
			}

			/*if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				m_camera->Move(eMovementType::Up);
			}

			if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				m_camera->Move(eMovementType::Down);
			}*/
		}

		// Draw mode toggles
		{
			if (key == GLFW_KEY_P && (action == GLFW_PRESS))
			{
				isWireframeEnabled = !isWireframeEnabled;

				if (isWireframeEnabled)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}

	void Window::OnMouseScroll(GLdouble xOffset, GLdouble yOffset)
	{
		/*const GLfloat scrollDirection = (yOffset < 0.0f) ? -1.0f : 1.0f;
		m_camera->Zoom(1.8f * scrollDirection);*/
		m_camera->ProcessMouseScroll(yOffset);
	}

	void Window::OnMouseMove(GLdouble xPos, GLdouble yPos)
	{
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		GLfloat xoffset = xPos - lastX;
		GLfloat yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

		lastX = xPos;
		lastY = yPos;

		m_camera->ProcessMouseMovement(xoffset, yoffset);
	}

	void Window::draw() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

		/*GLdouble mouseX, mouseY;
		glfwGetCursorPos(m_window.get(), &mouseX, &mouseY);
		glfwSetCursorPos(m_window.get(), m_width / 2.0f, m_height / 2.0f);
		m_camera->mouseX = static_cast<GLfloat>(mouseX);
		m_camera->mouseY = static_cast<GLfloat>(mouseY);

		m_camera->Update();*/

		glm::mat4 matProjection = glm::perspective(glm::radians(m_camera->Zoom), (GLfloat)m_width / (GLfloat)m_height, 0.3f, 1000.0f);
		glm::mat4 matView = m_camera->GetViewMatrix();
		glm::mat4 matModel;

		m_skyboxShader->Bind();
		m_skyboxShader->SetInt("skybox", 0);
		m_skyboxShader->SetMatrix4f("matView", matView);
		m_skyboxShader->SetMatrix4f("matProjection", matProjection);
		m_skyboxShader->Unbind();

		m_objShader->Bind();
		
		m_objShader->SetMatrix4f("matProjection", matProjection);
		m_objShader->SetMatrix4f("matView", matView);
		m_objShader->SetMatrix4f("matModel", matModel);

		m_objShader->SetFloat("ambientLightStrength", gAmbientLightStrength);
		//m_objShader->SetVec3("viewPosition", m_camera->position);
		m_objShader->SetVec3("viewPosition", m_camera->Position);
		m_objShader->SetInt("material.diffuse", 0);
		m_objShader->SetInt("material.specular", 1);
		m_objShader->SetFloat("material.shininess", 32.0f);

		// Set the scene's directional light source (sun)
		m_objShader->SetVec3("dirLight.direction", gDirLightDirection);
		m_objShader->SetVec3("dirLight.ambient", gDirLightAmbient);
		m_objShader->SetVec3("dirLight.diffuse", gDirLightDiffuse);
		m_objShader->SetVec3("dirLight.specular", gDirLightSpecular);

		// Set the camera's spotlight, reserve the first spotlight for the camera's
		m_objShader->SetInt("numSpotLights", 1);
		//m_objShader->SetVec3("spotLights[0].position", m_camera->position);
		//m_objShader->SetVec3("spotLights[0].direction", m_camera->right);
		m_objShader->SetVec3("spotLights[0].position", m_camera->Position);
		m_objShader->SetVec3("spotLights[0].direction", m_camera->Front);
		m_objShader->SetVec3("spotLights[0].ambient", gSpotLightAmbient);
		m_objShader->SetVec3("spotLights[0].diffuse", gSpotLightDiffuse);
		m_objShader->SetVec3("spotLights[0].specular", gSpotLightSpecular);
		m_objShader->SetFloat("spotLights[0].constant", gSpotLightConstant);
		m_objShader->SetFloat("spotLights[0].linear", gSpotLightLinear);
		m_objShader->SetFloat("spotLights[0].quadratic", gSpotLightQuadratic);
		m_objShader->SetFloat("spotLights[0].innerCutoff", gSpotLightInnerCutoff);
		m_objShader->SetFloat("spotLights[0].outerCutoff", gSpotLightOuterCutoff);
		
		m_scene->Draw();

		m_objShader->Unbind();

		glfwSwapBuffers(m_window.get());
		Time::Instance()->Update();
	}
}
