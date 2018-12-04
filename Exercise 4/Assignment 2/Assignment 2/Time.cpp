#include "Time.h"
#include "GLFW/glfw3.h"

namespace Graphics
{
	Time *Time::instance{ nullptr };

	Time::Time() : deltaTime{ 0.0f }, lastFrame{ 0.0f } { }

	Time* Time::Instance()
	{
		if (instance == nullptr)
			instance = new Time();

		return instance;
	}

	const GLfloat& Time::GetDeltaTime()
	{
		return deltaTime;
	}

	void Time::Update()
	{
		GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
}