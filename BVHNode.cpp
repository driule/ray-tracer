#include "precomp.h"

BVHNode::BVHNode()
{
	this->isLeaf = false;
}

bool BVHNode::intersects(Ray* ray)
{
	float tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;
	vec3 invertedDirection = vec3(1.0f / ray->direction.x, 1.0f / ray->direction.y, 1.0f / ray->direction.z);

	txmin = (this->boundingBox->min.x - ray->origin.x) * invertedDirection.x;
	txmax = (this->boundingBox->max.x - ray->origin.x) * invertedDirection.x;

	tymin = (this->boundingBox->min.y - ray->origin.y) * invertedDirection.y;
	tymax = (this->boundingBox->max.y - ray->origin.y) * invertedDirection.y;

	tzmin = (this->boundingBox->min.z - ray->origin.z) * invertedDirection.z;
	tzmax = (this->boundingBox->max.z - ray->origin.z) * invertedDirection.z;

	tmin = min(txmin, txmax);
	tmax = max(txmin, txmax);

	tmin = max(tmin, min(tymin, tymax));
	tmax = min(tmax, max(tymin, tymax));

	tmin = max(tmin, min(tzmin, tzmax));
	tmax = min(tmax, max(tzmin, tzmax));

	// early out
	if (tmin > ray->t)
		return false;

	return tmax >= tmin && tmax >= 0;
}

void BVHNode::translate(vec3 vector)
{
	this->boundingBox->translate(vector);

	if (this->isLeaf) return;

	this->left->translate(vector);
	this->right->translate(vector);
}
