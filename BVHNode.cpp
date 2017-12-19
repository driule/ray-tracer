#include "precomp.h"

BVHNode::BVHNode()
{
	this->isLeaf = false;
}

bool BVHNode::intersects(Ray* ray)
{
	float tmin = (this->boundingBox->min.x - ray->origin.x) / ray->direction.x;
	float tmax = (this->boundingBox->max.x - ray->origin.x) / ray->direction.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (this->boundingBox->min.y - ray->origin.y) / ray->direction.y;
	float tymax = (this->boundingBox->max.y - ray->origin.y) / ray->direction.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (this->boundingBox->min.z - ray->origin.z) / ray->direction.z;
	float tzmax = (this->boundingBox->max.z - ray->origin.z) / ray->direction.z;

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

void BVHNode::translate(vec3 vector)
{
	this->boundingBox->translate(vector);

	if (this->isLeaf) return;

	this->left->translate(vector);
	this->right->translate(vector);
}
