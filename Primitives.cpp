#include "precomp.h"

Material::Material(vec4 color)
{
	this->color = color;
}

Primitive::Primitive(Material* material)
{
	this->material = material;
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, vec3 position, int radius) : Primitive(material)
{
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

	if (p2 > this->radius2) return false;

	/*t = sqrt(this->radius2 - p2);
	if ((t < ray->t) && (t > 0))
	{
		ray->t = t;
	}*/

	return true;
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, vec3 a, vec3 b, vec3 c) : Primitive(material)
{
	this->a = a;
	this->b = b;
	this->c = c;
}

bool Triangle::intersects(Ray* ray)
{
	float t, u, v;

	vec3 ab = this->b - this->a;
	vec3 ac = this->c - this->a;
	vec3 pvec = ray->direction.cross(ac);
	float det = ab.dot(pvec);

	float invDet = 1 / det;

	vec3 tvec = ray->origin - a;
	u = tvec.dot(pvec) * invDet;

	if (u < 0 || u > 1) return false;

	vec3 qvec = tvec.cross(ab);
	v = ray->direction.dot(qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	//t = ac.dot(qvec) * invDet;
	//vec3 newRay = vec3(ray->origin + ray->direction * t);
	return true;
}
