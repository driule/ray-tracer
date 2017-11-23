#include "precomp.h"


Primitive::Primitive()
{
}

Primitive::~Primitive()
{
}

Sphere::Sphere(vec3 position, int radius)
{
	this->position = position;
	this->radius = radius;
}
