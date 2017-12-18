#include "precomp.h"

BVHNode::BVHNode()
{
	this->isLeaf = false;
}

bool BVHNode::intersects(Ray* ray)
{
	float tmin = (this->boundingBoxMin.x - ray->origin.x) / ray->direction.x;
	float tmax = (this->boundingBoxMax.x - ray->origin.x) / ray->direction.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (this->boundingBoxMin.y - ray->origin.y) / ray->direction.y;
	float tymax = (this->boundingBoxMax.y - ray->origin.y) / ray->direction.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (this->boundingBoxMin.z - ray->origin.z) / ray->direction.z;
	float tzmax = (this->boundingBoxMax.z - ray->origin.z) / ray->direction.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	// early out
	if (tmin > ray->t)
		return false;

	return true;
}

void BVHNode::destroy()
{
	if (this->isLeaf)
	{
		delete this;
		return;
	}

	this->left->destroy();
	this->right->destroy();

	delete this;
}

void BVHNode::translate(vec3 vector)
{
	this->boundingBoxMin += vector;
	this->boundingBoxMax += vector;

	if (this->isLeaf) return;

	this->left->translate(vector);
	this->left->translate(vector);
}

float BVHNode::calculateSurfaceArea()
{
	vec3 diagonal = (this->boundingBoxMax - this->boundingBoxMin).absolute();

	return ((diagonal.x * diagonal.y) + (diagonal.x * diagonal.z) + (diagonal.z * diagonal.y)) * 2;
}