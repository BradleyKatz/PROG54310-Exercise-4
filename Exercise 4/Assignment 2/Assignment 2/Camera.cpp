#include "Camera.h"
#include "Time.h"
#include <glm\gtc\constants.hpp>
#include <GLFW\glfw3.h>

namespace Graphics
{
	// The camera tends to invert open itself beyond 90 degrees, so cut its movement off just before that point
	const GLfloat Camera::MAX_PITCH{ 89.0f };
	const GLfloat Camera::MIN_PITCH{ -89.0f };
	const GLfloat Camera::MOUSE_SENSITIVITY{ 0.005f };

	Graphics::Camera::Camera()
		: zoom{45.0f}, movementSpeed{30.0f}, near{0.3f}, far{1000.0f}, width{0.0f}, height{0.0f},
		mouseX{0.0f}, mouseY{0.0f}, position{1.4f, 3.3f, 29.5f}, direction{0.0f, 0.0f, 0.0f}, right{0.0f, 0.0f, 0.0f}, up{0.0f, 0.0f, 0.0f}, matView{},
		matModel{1.0f}
	{
	}

	Camera::~Camera()
	{
	}

	const glm::mat4& Camera::GetViewMatrix() const { return matView; }
	const glm::mat4& Camera::GetProjectionMatrix() const { return matProjection; }
	const glm::mat4& Camera::GetModelMatrix() const { return matModel; }

	void Camera::Move(eMovementType movementType)
	{
		switch (movementType)
		{
		case eMovementType::Forwards:
			position += direction * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		case eMovementType::Backwards:
			position -= direction * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		case eMovementType::Left:
			position -= right * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		case eMovementType::Right:
			position += right * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		case eMovementType::Up:
			position += up * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		case eMovementType::Down:
			position -= up * Time::Instance()->GetDeltaTime() * movementSpeed;
			break;
		}

		return;
	}

	void Camera::Zoom(GLfloat zoomOffset)
	{
		zoom += zoomOffset;
	}

	void Camera::Update()
	{
		static GLfloat mouseXAngle{ glm::pi<GLfloat>() }, mouseYAngle{ 0.0f };
		static glm::mat4 matTranslation{ 1.0f };
		static glm::mat4 matProjection;

		mouseXAngle += MOUSE_SENSITIVITY * ((width / 2.0f) - mouseX);
		mouseYAngle += MOUSE_SENSITIVITY * ((height / 2.0f) - mouseY);

		direction = glm::vec3
		{
			cos(mouseYAngle) * sin(mouseXAngle),
			sin(mouseYAngle),
			cos(mouseYAngle) * cos(mouseXAngle)
		};

		right = glm::vec3
		{
			sin(mouseXAngle - glm::half_pi<GLfloat>()),
			0.0f,
			cos(mouseXAngle - glm::half_pi<GLfloat>())
		};

		up = glm::cross(right, direction);

		matView = glm::lookAt
		(
			position,
			position + direction,
			up
		);

		// Apply scroll wheel zoom
		matProjection = glm::perspective(glm::radians(zoom), width / height, near, far);
	}
}