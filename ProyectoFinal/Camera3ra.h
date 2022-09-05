#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera3ra
{
public:
	Camera3ra();
	Camera3ra(glm::vec3 startPosition, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getAvatarPosition();

	GLfloat getYaw() { return yaw; };
	GLfloat getPitch() { return pitch; };

	~Camera3ra();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

