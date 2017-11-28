#include "precomp.h"

Camera::Camera()
{
	this->position = vec3(0, 0, 0);
	this->viewDirection = vec3(0, 0, 1);
	this->fieldOfView = 10;

	vec3 screenCenter = this->position + this->fieldOfView * this->viewDirection;

	this->topLeft = screenCenter + vec3(-SCRWIDTH / 2, -SCRHEIGHT / 2, 0);
	this->topRight = screenCenter + vec3(SCRWIDTH / 2, -SCRHEIGHT / 2, 0);
	this->bottomLeft = screenCenter + vec3(-SCRWIDTH / 2, SCRHEIGHT / 2, 0);
}

vec3 Camera::generateRay(int x, int y)
{
	vec3 ray = this->topLeft + x * (this->topRight - this->topLeft) + y * (this->bottomLeft - this->topLeft);

	return normalize(ray);
}