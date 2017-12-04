#include "precomp.h"

#define ORIGIN vec3(0, 0, 0)

Camera::Camera()
{
	this->position = ORIGIN;
	this->viewDirection = vec3(0, 0, 1);
	this->fieldOfView = 0.5;

	this->calculateScreen();
}

void Camera::calculateScreen()
{
	vec3 screenCenter = ORIGIN + this->fieldOfView * this->viewDirection;

	this->topLeft = screenCenter + vec3(-1, -ASPECT_RATIO, 0);
	this->topRight = screenCenter + vec3(1, -ASPECT_RATIO, 0);
	this->bottomLeft = screenCenter + vec3(-1, ASPECT_RATIO, 0);
}

Ray* Camera::generateRay(int x, int y)
{
	Ray* ray = new Ray();
	ray->direction = normalize(
		(this->topLeft + (x / SCRWIDTHf) * (this->topRight - this->topLeft) + (y / SCRHEIGHTf) * (this->bottomLeft - this->topLeft)) - this->position
	);
	ray->origin = this->position;

	return ray;
}
