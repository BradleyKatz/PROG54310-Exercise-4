#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <glm\gtc\matrix_transform.hpp>

namespace Graphics
{
	enum class eMovementType
	{
		Forwards,
		Backwards,
		Left,
		Right,
		Up,
		Down,
	};

	class Camera
	{
	private:
		static const GLfloat MIN_PITCH, MAX_PITCH;
		static const GLfloat MOUSE_SENSITIVITY;

		GLfloat zoom;
		GLfloat movementSpeed;
		GLfloat near, far;
		GLfloat width, height;
		GLfloat mouseX, mouseY;
		glm::vec3 position, direction, right, up;
		glm::mat4 matView;
		glm::mat4 matProjection;
		glm::mat4 matModel;

	public:
		Camera();
		~Camera();

		const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetModelMatrix() const;
		void Move(eMovementType movementType);
		void Zoom(GLfloat zoomOffset);
		void Update();

		friend class Window;
	};
}