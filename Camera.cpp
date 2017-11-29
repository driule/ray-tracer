#include "precomp.h"

Camera::Camera()
{
	this->position = vec3(0, 0, 0);
	this->viewDirection = vec3(0, 0, 1);
	this->fieldOfView = 250;

	vec3 screenCenter = this->position + this->fieldOfView * this->viewDirection;

	//this->topLeft = screenCenter + vec3(-SCRWIDTH / 2, -SCRHEIGHT / 2, 0);
	//this->topRight = screenCenter + vec3(SCRWIDTH / 2, -SCRHEIGHT / 2, 0);
	//this->bottomLeft = screenCenter + vec3(-SCRWIDTH / 2, SCRHEIGHT / 2, 0);

	this->topLeft = screenCenter + vec3(-1, -1, 1);
	this->topRight = screenCenter + vec3(1, -1, 1);
	this->bottomLeft = screenCenter + vec3(-1, 1, 1);
}

Ray* Camera::generateRay(int x, int y)
{
	Ray* ray = new Ray();
	ray->direction = normalize(this->topLeft + x * (this->topRight - this->topLeft) + y * (this->bottomLeft - this->topLeft));
	ray->origin = this->position;
	ray->t = INFINITY;

	return ray;
}
