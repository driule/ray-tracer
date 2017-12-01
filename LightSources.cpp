#include "precomp.h"


LightSource::LightSource(vec3 position, int id)
{
	this->position = position;
	this->id = id;
}

void LightSource::intersect(Ray* ray)
{
	vec3 c = this->position - ray->origin;
	float t = dot(c, ray->direction);
	if (t < 0) return;
	vec3 q = c - t * ray->direction;
	if (q.x == 0 && q.y == 0 && q.z == 0)
	{
		ray->t = t;
		ray->intersectedPrimitiveId = this->id;
	}
}
