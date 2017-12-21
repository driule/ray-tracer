#include "precomp.h"

#define BINS_COUNT 4

TopBVH::TopBVH(std::vector<Primitive*> primitives, std::vector<BVH*> BVHs) : BVH(primitives)
{
	this->BVHs = BVHs;

	// build primitive and BVH indices array
	this->primitiveIndices = new int[this->primitives.size()];
	this->objectIndices = new int[BVHs.size()];

	int count = 0;
	for (int i = 0; i < this->BVHs.size(); i++)
	{
		for (int j = this->BVHs[i]->root->first; j < this->BVHs[i]->root->first + this->BVHs[i]->root->count; j++)
		{
			this->primitiveIndices[count] = this->BVHs[i]->objectIndices[j];
			count++;
		}

		// create BVH indices array
		this->objectIndices[i] = BVHs[i]->id;

	}

	// fill bounding boxes vector
	for (int i = 0; i < this->BVHs.size(); i++)
	{
		this->boundingBoxes.push_back(this->BVHs[i]->root->boundingBox);
	}

	// create root
	this->root = new BVHNode();
	this->root->first = 0;
	this->root->count = this->BVHs.size();

	this->nodes.push_back(this->root);

	this->calculateBounds(this->root);
	this->subdivide(this->root);
}

void TopBVH::subdivide(BVHNode* node)
{
	if (node->count == 1)
	{
		int index = this->objectIndices[node->first];

		node->first = this->BVHs[index]->root->first;
		node->count = this->BVHs[index]->root->count;

		node->left = this->BVHs[index]->root->left;
		node->right = this->BVHs[index]->root->right;

		node->isLeaf = this->BVHs[index]->root->isLeaf;

		return;
	}

	node->left = new BVHNode();
	node->right = new BVHNode();
	
	this->nodes.push_back(node->left);
	this->nodes.push_back(node->right);

	this->partition(node, BINS_COUNT);

	this->subdivide(node->left);
	this->subdivide(node->right);
}

void TopBVH::traverse(BVHNode* node, Ray* ray, bool isShadowRay)
{
	if (!node->intersects(ray))
		return;

	if (isShadowRay && ray->intersectedObjectId != -1)
		return;

	if (node->isLeaf)
	{
		// intersect primitves
		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->primitiveIndices[i];
			this->primitives[index]->intersect(ray);

			if (isShadowRay && ray->intersectedObjectId != -1)
				return;
		}
	}
	else
	{
		/*if (!isShadowRay)
		{
			float dxLeft = MAX(node->left->boundingBox->min.x - ray->origin.x, ray->origin.x - node->left->boundingBox->max.x);
			float dyLeft = MAX(node->left->boundingBox->min.y - ray->origin.y, ray->origin.y - node->left->boundingBox->max.y);
			float dzLeft = MAX(node->left->boundingBox->min.z - ray->origin.z, ray->origin.z - node->left->boundingBox->max.z);

			float dxRight = MAX(node->right->boundingBox->min.x - ray->origin.x, ray->origin.x - node->right->boundingBox->max.x);
			float dyRight = MAX(node->right->boundingBox->min.y - ray->origin.y, ray->origin.y - node->right->boundingBox->max.y);
			float dzRight = MAX(node->right->boundingBox->min.z - ray->origin.z, ray->origin.z - node->right->boundingBox->max.z);

			float distanceLeft = vec3(dxLeft, dyLeft, dzLeft).sqrLentgh();
			float distanceRight = vec3(dxRight, dyRight, dzRight).sqrLentgh();

			if (distanceLeft < distanceRight)
			{
				this->traverse(node->left, ray, isShadowRay);
				//printf("DistanceLeft: %f, DistanceRight: %f, ray->t: %f \n", distanceLeft, distanceRight, ray->t);
				if (ray->t * ray->t > distanceRight)
					this->traverse(node->right, ray, isShadowRay);
			}
			else
			{
				this->traverse(node->right, ray, isShadowRay);
				//printf("DistanceLeft: %f, DistanceRight: %f, ray->t: %f \n", distanceLeft, distanceRight, ray->t);
				if (ray->t * ray->t > distanceLeft)
					this->traverse(node->left, ray, isShadowRay);
			}
		}
		else
		{
			this->traverse(node->left, ray, isShadowRay);
			this->traverse(node->right, ray, isShadowRay);
		}*/
		this->traverse(node->left, ray, isShadowRay);
		this->traverse(node->right, ray, isShadowRay);
	}
}

TopBVH::~TopBVH()
{
	for (int i = 0; i < this->nodes.size(); i++)
	{
		delete this->nodes[i]->boundingBox;
		delete this->nodes[i];
	}

	delete this->primitiveIndices;
	delete this->objectIndices;
}