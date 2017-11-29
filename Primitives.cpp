#include "precomp.h"

Material::Material(vec4 color)
{
	this->color = color;
}

Primitive::Primitive()
{
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, vec3 position, int radius)
{
	this->material = material;
	this->position = position;
	this->radius = radius;
	this->radius2 = radius * radius;
}

bool Sphere::intersects(Ray* ray)
{
	vec3 c = this->position - ray->origin;
	float t = dot(c, ray->direction);
	vec3 q = c - t * ray->direction;
	float p2 = dot(q, q);

	if (p2 > this->radius2)
	{
		return false;
	}

	t = sqrt(this->radius2 - p2);
	if ((t < ray->t) && (t > 0))
	{
		ray->t = t;
	}

	return true;
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, vec3 v1, vec3 v2, vec3 v3)
{
	this->material = material;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
}

bool Triangle::intersects(Ray* ray)
{
	return false;
}
