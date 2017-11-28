#include "precomp.h"

Material::Material(vec4 color)
{
	this->color = color;
}

Primitive::Primitive()
{
}

Sphere::Sphere(Material* material, vec3 position, int radius)
{
	this->material = material;
	this->position = position;
	this->radius = radius;
}
