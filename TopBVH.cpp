#include "precomp.h"

TopBVH::TopBVH(std::vector<Primitive*> primitives, std::vector<BVH*> BVHs)
{
	this->primitives = primitives;
	this->BVHs = BVHs;

	// build primitive indices
	this->primitiveIndices = new int[this->primitives.size()];
	this->BVHsIndices = new int[BVHs.size()];
	int count = 0;
	for (int i = 0; i < this->BVHs.size(); i++)
	{
		for (int j = this->BVHs[i]->root->first; j < this->BVHs[i]->root->first + this->BVHs[i]->root->count; j++)
		{
			this->primitiveIndices[count] = this->BVHs[i]->primitiveIndices[j];
			count++;
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

		node->first = this->BVHs[index]->root->first;
		node->count = this->BVHs[index]->root->count;

		node->left = this->BVHs[index]->root->left;
		node->right = this->BVHs[index]->root->right;

		node->isLeaf = this->BVHs[index]->root->isLeaf;

		return;
	}

	node->left = new BVHNode();
	node->right = new BVHNode();

	this->partition(node);

	this->subdivide(node->left);
	this->subdivide(node->right);
}

void TopBVH::partition(BVHNode* node)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 0, optimalRightCount = 0;

	int* optimalBVHIndices = new int[node->count];
	for (int i = 0; i < node->count; i++)
	{
		optimalBVHIndices[i] = this->BVHsIndices[node->first + i];
	}

	int binCount = 10;
	std::vector<int>* bins = new std::vector<int>[binCount];
	vec3 binWidth = (node->boundingBoxMax - node->boundingBoxMin) / binCount;

	for (int axis = 0; axis < 3; axis++)
	{
		for (int i = 0; i < binCount; i++) bins[i].clear();

		// divide primitives to bins
		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->BVHsIndices[i], binIndex;
			vec3 center = this->BVHs[index]->root->boundingBoxMin + 0.5 * (this->BVHs[index]->root->boundingBoxMax - this->BVHs[index]->root->boundingBoxMin);
			if (axis == 0)		binIndex = (center.x - node->boundingBoxMin.x) / binWidth.x;
			else if (axis == 1)	binIndex = (center.y - node->boundingBoxMin.y) / binWidth.y;
			else if (axis == 2)	binIndex = (center.z - node->boundingBoxMin.z) / binWidth.z;

			binIndex = MIN(binCount - 1, binIndex);
			bins[binIndex].push_back(index);
		}

		// sort primitive indices
		int count = 0;
		for (int i = 0; i < binCount; i++)
		{
			for (int j = 0; j < bins[i].size(); j++)
			{
				this->BVHsIndices[node->first + count] = bins[i][j];
				count++;
			}
		}

		// evaluate bin combinations
		for (int i = 0; i < binCount - 1; i++)
		{
			int leftCount = 0, rightCount = 0;
			for (int j = 0; j <= i; j++)
			{
				leftCount += bins[j].size();
			}
			rightCount = node->count - leftCount;

			if (leftCount == 0 || rightCount == 0) continue;

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
				for (int j = 0; j < node->count; j++)
				{
					optimalBVHIndices[j] = this->BVHsIndices[node->first + j];
				}
			}
		}
	}
	delete[] bins;

	// set optimal split values
	for (int i = 0; i < node->count; i++)
	{
		this->BVHsIndices[node->first + i] = optimalBVHIndices[i];
	}

	node->left->first = node->first;
	node->left->count = optimalLeftCount;
	calculateBounds(node->left);

	node->right->first = node->first + optimalLeftCount;
	node->right->count = optimalRightCount;
	calculateBounds(node->right);

	delete optimalBVHIndices;
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
