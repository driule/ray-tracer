#include "precomp.h"

#define MAX_PRIMITIVES 3
#define MAX_DEPTH 20
#define BINS_COUNT 10

BVH::BVH(std::vector<Primitive*> primitives)
{
	this->primitives = primitives;
}

void BVH::build(int id, int startIndex, int endIndex)
{
	this->id = id;

	this->objectIndices = new int[this->primitives.size()];
	for (int i = 0; i < this->primitives.size(); i++)
	{
		objectIndices[i] = i;
	}

	for (int i = 0; i < this->primitives.size(); i++)
	{
		this->boundingBoxes.push_back(this->primitives[i]->boundingBox);
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
		int index = this->objectIndices[i];
		minX = MIN(this->boundingBoxes[index]->min.x, minX);
		minY = MIN(this->boundingBoxes[index]->min.y, minY);
		minZ = MIN(this->boundingBoxes[index]->min.z, minZ);

		maxX = MAX(this->boundingBoxes[index]->max.x, maxX);
		maxY = MAX(this->boundingBoxes[index]->max.y, maxY);
		maxZ = MAX(this->boundingBoxes[index]->max.z, maxZ);
	}

	node->boundingBox->min = vec3(minX, minY, minZ);
	node->boundingBox->max = vec3(maxX, maxY, maxZ);
	node->boundingBox->calculateCenter();
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

	this->partition(node, BINS_COUNT);

	depth++;
	this->subdivide(node->left, depth);
	this->subdivide(node->right, depth);
}

void BVH::partition(BVHNode* node, int binCount)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 1;
	int optimalRightCount = node->count - optimalLeftCount;

	int* optimalObjectIndices = new int[node->count];
	for (int i = 0; i < node->count; i++)
	{
		optimalObjectIndices[i] = this->objectIndices[node->first + i];
	}

	std::vector<int>* bins = new std::vector<int>[binCount];
	vec3 binWidth = (node->boundingBox->max - node->boundingBox->min) / binCount;
	if (binWidth.x == 0) binWidth.x = 1;
	if (binWidth.y == 0) binWidth.y = 1;
	if (binWidth.z == 0) binWidth.z = 1;

	for (int axis = 0; axis < 3; axis++)
	{
		for (int i = 0; i < binCount; i++) bins[i].clear();

		// divide objects to bins
		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->objectIndices[i], binIndex;

			if (axis == 0)		binIndex = (this->boundingBoxes[index]->center.x - node->boundingBox->min.x) / binWidth.x;
			else if (axis == 1)	binIndex = (this->boundingBoxes[index]->center.y - node->boundingBox->min.y) / binWidth.y;
			else if (axis == 2)	binIndex = (this->boundingBoxes[index]->center.z - node->boundingBox->min.z) / binWidth.z;

			binIndex = MIN(binCount - 1, binIndex);
			bins[binIndex].push_back(index);
		}

		// sort objects
		int count = 0;
		for (int i = 0; i < binCount; i++)
		{
			for (int j = 0; j < bins[i].size(); j++)
			{
				this->objectIndices[node->first + count] = bins[i][j];
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
			float surfaceAreaLeft = node->left->boundingBox->calculateSurfaceArea();
			float surfaceAreaRight = node->right->boundingBox->calculateSurfaceArea();
			float SAH = surfaceAreaLeft * node->left->count + surfaceAreaRight * node->right->count;

			// save the optimal split according Surface Area Heuristic
			if (SAH < optimalSAH && SAH < (surfaceAreaLeft + surfaceAreaRight) * node->count)
			{
				optimalSAH = SAH;
				optimalLeftCount = leftCount;
				optimalRightCount = rightCount;

				for (int j = 0; j < node->count; j++)
				{
					optimalObjectIndices[j] = this->objectIndices[node->first + j];
				}
			}
		}
	}
	delete[] bins;

	// set optimal split values
	for (int i = 0; i < node->count; i++)
	{
		this->objectIndices[node->first + i] = optimalObjectIndices[i];
	}

	node->left->first = node->first;
	node->left->count = optimalLeftCount;
	calculateBounds(node->left);

	node->right->first = node->first + optimalLeftCount;
	node->right->count = optimalRightCount;
	calculateBounds(node->right);

	delete optimalObjectIndices;
}
