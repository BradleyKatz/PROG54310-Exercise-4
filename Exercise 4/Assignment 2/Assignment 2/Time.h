#pragma once
#include <GL\glew.h>

namespace Graphics
{
	class Time
	{
	private:
		static Time *instance;
		Time();

		GLfloat deltaTime, lastFrame;

	public:
		static Time* Instance();
		const GLfloat& GetDeltaTime();

		void Update();
	};
}