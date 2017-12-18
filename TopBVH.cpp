#include "precomp.h"

TopBVH::TopBVH(std::vector<Primitive*> primitives, std::vector<BVH*> BVHs)
{
	this->primitives = primitives;
	this->BVHs = BVHs;

	// build primitive indices
	int N = this->primitives.size();
	this->primitiveIndices = new int[N];
	this->BVHsIndices = new int[BVHs.size()];
	int count = 0;
	for (int i = 0; i < this->BVHs.size(); i++)
	{
		for (int j = this->BVHs[i]->root->first; j < this->BVHs[i]->root->first + this->BVHs[i]->root->count; j++)
		{
			this->primitiveIndices[count++] = this->BVHs[i]->primitiveIndices[this->BVHs[i]->root->first + j];
		}

		// create BVHs indices array
		this->BVHsIndices[i] = BVHs[i]->id;

	}

	// create root
	this->root = new BVHNode();
	this->root->first = 0;
	this->root->count = this->BVHs.size();

	this->calculateBounds(this->root);
	this->subdivide(this->root);
}

void TopBVH::calculateBounds(BVHNode* node)
{
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;
	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		int index = this->BVHsIndices[i];
		minX = MIN(this->BVHs[index]->root->boundingBoxMin.x, minX);
		minY = MIN(this->BVHs[index]->root->boundingBoxMin.y, minY);
		minZ = MIN(this->BVHs[index]->root->boundingBoxMin.z, minZ);

		maxX = MAX(this->BVHs[index]->root->boundingBoxMax.x, maxX);
		maxY = MAX(this->BVHs[index]->root->boundingBoxMax.y, maxY);
		maxZ = MAX(this->BVHs[index]->root->boundingBoxMax.z, maxZ);
	}

	node->boundingBoxMin = vec3(minX, minY, minZ);
	node->boundingBoxMax = vec3(maxX, maxY, maxZ);
}

void TopBVH::subdivide(BVHNode* node)
{
	if (node->count == 1)
	{
		int index = this->BVHsIndices[node->first];
		node = this->BVHs[index]->root;
		
		return;
	}

	node->left = new BVHNode();
	node->left->parent = node;

	node->right = new BVHNode();
	node->right->parent = node;

	this->randomPartition(node);

	this->subdivide(node->left);
	this->subdivide(node->right);
}

void TopBVH::randomPartition(BVHNode* node)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 0, optimalRightCount = 0;
	for (int i = 0; i < 5; i++)
	{
		int leftCount = std::rand() % node->count;
		int rightCount = node->count - leftCount;

		node->left->first = node->first;
		node->left->count = leftCount;
		calculateBounds(node->left);

		node->right->first = node->first + leftCount;
		node->right->count = rightCount;
		calculateBounds(node->right);

		// calculate surface area
		float surfaceAreaLeft = node->left->calculateSurfaceArea();
		float surfaceAreaRight = node->right->calculateSurfaceArea();
		float SAH = surfaceAreaLeft * node->left->count + surfaceAreaRight * node->right->count;

		// save the optimal split according Surface Area Heuristic
		if (SAH < optimalSAH && SAH < (surfaceAreaLeft + surfaceAreaRight) * node->count)
		{
			optimalSAH = SAH;
			optimalLeftCount = leftCount;
			optimalRightCount = rightCount;
		}
	}

	// set optimal split values
	node->left->first = node->first;
	node->left->count = optimalLeftCount;
	calculateBounds(node->left);

	node->right->first = node->first + optimalLeftCount;
	node->right->count = optimalRightCount;
	calculateBounds(node->right);
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
		this->traverse(node->left, ray, isShadowRay);
		this->traverse(node->right, ray, isShadowRay);
	}
}
