#include "precomp.h"

Material::Material(vec4 color, MaterialType type)
{
	this->color = color;
	this->type = type;
}

Primitive::Primitive(Material* material, int id)
{
	this->material = material;
	this->id = id;
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, int id, vec3 position, float radius) : Primitive(material, id)
{
	this->position = position;
	this->radius = radius;
	this->radius2 = radius * radius;
}

void Sphere::intersect(Ray* ray)
{
	vec3 c = this->position - ray->origin;
	float t = dot(c, ray->direction);
	if (t < 0) return;
	vec3 q = c - t * ray->direction;
	float p2 = dot(q, q);

	if (p2 > this->radius2) return;

	t -= sqrt(this->radius2 - p2);
	if ((t < ray->t) && t > 0)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, int id, vec3 a, vec3 b, vec3 c) : Primitive(material, id)
{
	this->a = a;
	this->b = b;
	this->c = c;
}

void Triangle::intersect(Ray* ray)
{
	float t, u, v;

	vec3 ab = this->b - this->a;
	vec3 ac = this->c - this->a;
	vec3 pvec = ray->direction.cross(ac);
	float det = ab.dot(pvec);

	float invDet = 1 / det;

	vec3 tvec = ray->origin - a;
	u = tvec.dot(pvec) * invDet;

	if (u < 0 || u > 1) return;

	vec3 qvec = tvec.cross(ab);
	v = ray->direction.dot(qvec) * invDet;
	if (v < 0 || u + v > 1) return;

	t = ac.dot(qvec) * invDet;
	if (t < ray->t && t > 0)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}
