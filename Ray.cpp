#include "precomp.h"


Ray::Ray()
{
}

Ray::Ray(vec3 origin, vec3 direction, float t)
{
	this->origin = origin;
	this->direction = direction;
	this->t = t;
}
