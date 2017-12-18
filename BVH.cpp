#include "precomp.h"

#define MAX_PRIMITIVES 3
#define MAX_DEPTH 20

BVH::BVH(int id, std::vector<Primitive*> primitives)
{
	this->id = id;
	this->primitives = primitives;
}

void BVH::createBVH(int startIndex, int endIndex)
{
	int N = this->primitives.size();
	this->primitiveIndices = new int[N];
	for (int i = 0; i < N; i++)
	{
		primitiveIndices[i] = i;
	}

	this->root = new BVHNode();
	this->root->first = startIndex;
	this->root->count = endIndex - startIndex + 1;

	calculateBounds(this->root);
	subdivide(this->root, 0);
}

void BVH::calculateBounds(BVHNode* node)
{
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;
	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		int index = this->primitiveIndices[i];
		minX = MIN(this->primitives[index]->boundingBoxMin.x, minX);
		minY = MIN(this->primitives[index]->boundingBoxMin.y, minY);
		minZ = MIN(this->primitives[index]->boundingBoxMin.z, minZ);

		maxX = MAX(this->primitives[index]->boundingBoxMax.x, maxX);
		maxY = MAX(this->primitives[index]->boundingBoxMax.y, maxY);
		maxZ = MAX(this->primitives[index]->boundingBoxMax.z, maxZ);
	}

	node->boundingBoxMin = vec3(minX, minY, minZ);
	node->boundingBoxMax = vec3(maxX, maxY, maxZ);
}

void BVH::subdivide(BVHNode* node, int depth)
{
	if (node->count <= MAX_PRIMITIVES || depth >= MAX_DEPTH)
	{
		node->isLeaf = true;
		return;
	}

	node->left = new BVHNode();
	node->right = new BVHNode();

	this->partition(node);

	depth++;
	this->subdivide(node->left, depth);
	this->subdivide(node->right, depth);
}

void BVH::partition(BVHNode* node)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 0, optimalRightCount = 0;

	int* optimalPrimitiveIndices = new int[node->count];
	for (int i = 0; i < node->count; i++)
	{
		optimalPrimitiveIndices[i] = this->primitiveIndices[node->first + i];
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
			int index = this->primitiveIndices[i], binIndex;

			if (axis == 0)		binIndex = (this->primitives[index]->center.x - node->boundingBoxMin.x) / binWidth.x;
			else if (axis == 1)	binIndex = (this->primitives[index]->center.y - node->boundingBoxMin.y) / binWidth.y;
			else if (axis == 2)	binIndex = (this->primitives[index]->center.z - node->boundingBoxMin.z) / binWidth.z;

			binIndex = MIN(binCount - 1, binIndex);
			bins[binIndex].push_back(index);
		}

		// sort primitive indices
		int count = 0;
		for (int i = 0; i < binCount; i++)
		{
			for (int j = 0; j < bins[i].size(); j++)
			{
				this->primitiveIndices[node->first + count] = bins[i][j];
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
					optimalPrimitiveIndices[j] = this->primitiveIndices[node->first + j];
				}
			}
		}
	}
	delete[] bins;

	// set optimal split values
	for (int i = 0; i < node->count; i++)
	{
		this->primitiveIndices[node->first + i] = optimalPrimitiveIndices[i];
	}

	node->left->first = node->first;
	node->left->count = optimalLeftCount;
	calculateBounds(node->left);

	node->right->first = node->first + optimalLeftCount;
	node->right->count = optimalRightCount;
	calculateBounds(node->right);

	delete optimalPrimitiveIndices;
}
