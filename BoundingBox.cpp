#include "precomp.h"

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(vec3 min, vec3 max)
{
	this->min = min;
	this->max = max;

	this->calculateCenter();
}

void BoundingBox::calculateCenter()
{
	this->center = this->min + 0.5 * (this->max - this->min);
}

void BoundingBox::translate(vec3 vector)
{
	this->min += vector;
	this->max += vector;

	this->center += vector;
}

float BoundingBox::calculateSurfaceArea()
{
	vec3 diagonal = (this->max - this->min).absolute();

	return ((diagonal.x * diagonal.y) + (diagonal.x * diagonal.z) + (diagonal.z * diagonal.y)) * 2;
}

