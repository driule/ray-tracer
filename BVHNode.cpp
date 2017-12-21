#include "precomp.h"

BVHNode::BVHNode()
{
	this->isLeaf = false;
	this->boundingBox = new BoundingBox();
}

bool BVHNode::intersects(Ray* ray)
{
	float tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;

	txmin = (this->boundingBox->min.x - ray->origin.x) * ray->invertedDirection.x;
	txmax = (this->boundingBox->max.x - ray->origin.x) * ray->invertedDirection.x;

	tymin = (this->boundingBox->min.y - ray->origin.y) * ray->invertedDirection.y;
	tymax = (this->boundingBox->max.y - ray->origin.y) * ray->invertedDirection.y;

	tzmin = (this->boundingBox->min.z - ray->origin.z) * ray->invertedDirection.z;
	tzmax = (this->boundingBox->max.z - ray->origin.z) * ray->invertedDirection.z;

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

void BVHNode::destroy()
{
	if (this->isLeaf)
	{
		delete this->boundingBox;
		delete this;

		return;
	}

	this->left->destroy();
	this->right->destroy();

	delete this->boundingBox;
	delete this;
}