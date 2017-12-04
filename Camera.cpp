#include "precomp.h"

#define ORIGIN vec3(0, 0, 0)

Camera::Camera()
{
	this->position = ORIGIN;
	this->viewDirection = vec3(0, 0, 1);
	this->fieldOfView = 1;
	this->up = vec3(0, -1, 0);
	this->right = vec3(1, 0, 0);

	this->calculateScreen();
}

void Camera::calculateScreen()
{
	this->viewDirectionNormalized = normalize(this->viewDirection - this->position);
	this->right = cross(this->up, this->viewDirectionNormalized);
	this->up = cross(this->viewDirectionNormalized, this->right);

	vec3 center = this->position + this->fieldOfView * this->viewDirectionNormalized;
	//vec3 screenCenter = this->position + this->fieldOfView * this->viewDirection;

	this->topLeft = center -  this->right + this->up * ASPECT_RATIO;
	this->topRight = center + this->right + this->up * ASPECT_RATIO;
	this->bottomLeft = center - this->right - this->up * ASPECT_RATIO;
 
	//this->topLeft = screenCenter + vec3(-1, -ASPECT_RATIO, 1);
	//this->topRight = screenCenter + vec3(1, -ASPECT_RATIO, 1);
	//this->bottomLeft = screenCenter + vec3(-1, ASPECT_RATIO, 1);
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
