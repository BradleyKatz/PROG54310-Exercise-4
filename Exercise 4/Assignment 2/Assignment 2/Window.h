#pragma once
#include <memory>
#include <string>
#include <vector>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Vertex.h"
#include "Shader.h"
#include "Scene.h"
//#include "Camera.h"
#include <learnopengl\camera.h>

namespace Graphics
{
	// Implement the GLFW window deleter as a so-called "functor"
	struct GLFWDeleter
	{
		void operator()(GLFWwindow *ptr) { glfwDestroyWindow(ptr); }
	};

	typedef std::unique_ptr<GLFWwindow, GLFWDeleter> SmartGLFWwindow;

	class Window
	{
	public:
		Window() = delete;
		Window(int width, int height, const std::string& title);
		~Window();

		void LoadScene(Scene *scene);
		void run();

	protected:
		static void error_cb(int error, const char* description);
		static void keyboard_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void scroll_cb(GLFWwindow *window, GLdouble xOffset, GLdouble yOffset);
		static void mouse_cb(GLFWwindow *window, GLdouble xPos, GLdouble yPos);
		void keyPress(int key, int scancode, int action, int mods);
		void OnMouseScroll(GLdouble xOffset, GLdouble yOffset);
		void OnMouseMove(GLdouble xPos, GLdouble yPos);
		void draw() const;

		// Window specific data
		GLdouble m_width;
		GLdouble m_height;
		std::string m_title;
		SmartGLFWwindow m_window;

		// Scene Data
		Shader *m_objShader;
		Shader *m_skyboxShader;
		Scene *m_scene;
		Camera *m_camera;
		
		GLfloat lastX, lastY;
		GLboolean firstMouse;

		// Input related data
		static const GLdouble MOUSE_SENSITIVITY;
		GLdouble prevMouseX, prevMouseY;
	};
}

