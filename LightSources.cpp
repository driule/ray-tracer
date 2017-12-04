#include "precomp.h"


LightSource::LightSource(int id, vec3 position, vec4 color)
{
	this->id = id;
	this->position = position;
	this->color = color;
}

// -------------------- DIRECT LIGHT ------------------------------------

DirectLight::DirectLight(int id, vec3 position, vec4 color) : LightSource(id, position, color)
{
}

void DirectLight::intersect(Ray* ray)
{
	vec3 c = this->position - ray->origin;
	float t = dot(c, ray->direction);
	if (t < 0) return;
	vec3 q = c - t * ray->direction;
	if (q.x == 0 && q.y == 0 && q.z == 0)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}
