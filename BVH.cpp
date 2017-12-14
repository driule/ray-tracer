#include "precomp.h"

#define MAX_PRIMITIVES 3
#define MAX_DEPTH 20

BVH::BVH(std::vector<Primitive*> primitives)
{
	this->primitives = primitives;
	int N = this->primitives.size();
	this->primitiveIndices = new int[N];
	for (int i = 0; i < N; i++)
	{
		primitiveIndices[i] = i;
	}

	this->root = new Node();
	this->root->first = 0;
	this->root->count = N;

	calculateBounds(this->root);
	subdivide(this->root, 0);
}

void BVH::subdivide(Node* node, int depth)
{
	if (node->count <= MAX_PRIMITIVES || depth >= MAX_DEPTH)
	{
		/*printf("Leaf (depth: %i). first: %i, count: %i", depth, node->first, node->count);
		for (int i = 0; i < node->count; i++)
		{
		int index = this->primitiveIndices[node->first + i];
		printf(" |(%i) %f,%f,%f |", index, this->primitives[index]->center.x, this->primitives[index]->center.y, this->primitives[index]->center.z);
		}
		printf("\n\n");
		//*/
		node->isLeaf = true;
		return;
	}

	node->left = new Node();
	node->right = new Node();
	this->partition(node);
	//this->randomPartition(node);

	depth++;
	this->subdivide(node->left, depth);
	this->subdivide(node->right, depth);
}

void BVH::calculateBounds(Node* node)
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

void BVH::partition(Node* node)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 0, optimalRightCount = 0;

	int* optimalPrimitiveIndices = new int[this->primitives.size()];
	memcpy(optimalPrimitiveIndices, this->primitiveIndices, this->primitives.size() * sizeof(int));

	// try 3 different splits along x, y, z axes
	vec3 splitPlane = node->boundingBoxMin + 0.5 * node->boundingBoxMax;
	for (int j = 0; j < 3; j++)
	{
		int* nodePrimitiveIndices = new int[node->count];
		int leftCount = 0, rightCount = 0;

		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->primitiveIndices[i];
			bool assignedToLeftNode = false;
			if (j == 0)			assignedToLeftNode = this->primitives[index]->center.x < splitPlane.x;
			else if (j == 1)	assignedToLeftNode = this->primitives[index]->center.y < splitPlane.y;
			else if (j == 2)	assignedToLeftNode = this->primitives[index]->center.z < splitPlane.z;

			if (assignedToLeftNode)
			{
				nodePrimitiveIndices[leftCount] = index;
				leftCount++;
			}
			else
			{
				rightCount++;
				nodePrimitiveIndices[node->count - rightCount] = index;
			}
		}

		// update current node state
		for (int i = 0; i < node->count; i++)
		{
			this->primitiveIndices[node->first + i] = nodePrimitiveIndices[i];
		}

		node->left->first = node->first;
		node->left->count = leftCount;
		calculateBounds(node->left);

		node->right->first = node->first + leftCount;
		node->right->count = rightCount;
		calculateBounds(node->right);

		// calculate surface area
		float surfaceAreaLeft = this->calculateSurfaceArea(node->left);
		float surfaceAreaRight = this->calculateSurfaceArea(node->right);
		float SAH = surfaceAreaLeft * node->left->count + surfaceAreaRight * node->right->count;

		// save the optimal split according Surface Area Heuristic
		if (SAH < optimalSAH && SAH < (surfaceAreaLeft + surfaceAreaRight) * node->count)
		{
			optimalSAH = SAH;
			optimalLeftCount = leftCount;
			optimalRightCount = rightCount;
			memcpy(optimalPrimitiveIndices, this->primitiveIndices, this->primitives.size() * sizeof(int));
		}

		delete nodePrimitiveIndices;
	}

	memcpy(this->primitiveIndices, optimalPrimitiveIndices, this->primitives.size() * sizeof(int));

	// set optimal split values
	node->left->first = node->first;
	node->left->count = optimalLeftCount;
	calculateBounds(node->left);

	node->right->first = node->first + optimalLeftCount;
	node->right->count = optimalRightCount;
	calculateBounds(node->right);

	delete optimalPrimitiveIndices;
}

void BVH::randomPartition(Node* node)
{
	float optimalSAH = INFINITY;
	int optimalLeftCount = 0, optimalRightCount = 0;
	for (int j = 0; j < 50; j++)
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
		float surfaceAreaLeft = this->calculateSurfaceArea(node->left);
		float surfaceAreaRight = this->calculateSurfaceArea(node->right);
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

float BVH::calculateSurfaceArea(Node* node)
{
	vec3 diagonal = (node->boundingBoxMax - node->boundingBoxMin).absolute();

	return ((diagonal.x * diagonal.y) + (diagonal.x * diagonal.z) + (diagonal.z * diagonal.y)) * 2;
}

void BVH::traverse(Node* node, Ray* ray)
{
	if (!this->intersects(node, ray))
	{
		return;
	}
	if (node->isLeaf)
	{
		//printf("traverse leaf, count %i, first: %i\n", node->count, node->first);
		// intersect primitves
		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->primitiveIndices[i];
			this->primitives[index]->intersect(ray);
		}
	}
	else
	{
		//printf("traverse not leaf, count %i\n", node->count);
		this->traverse(node->left, ray);
		this->traverse(node->right, ray);
	}
}

bool BVH::intersects(Node* node, Ray* ray)
{
	float tmin = (node->boundingBoxMin.x - ray->origin.x) / ray->direction.x;
	float tmax = (node->boundingBoxMax.x - ray->origin.x) / ray->direction.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (node->boundingBoxMin.y - ray->origin.y) / ray->direction.y;
	float tymax = (node->boundingBoxMax.y - ray->origin.y) / ray->direction.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false; // No intersection

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (node->boundingBoxMin.z - ray->origin.z) / ray->direction.z;
	float tzmax = (node->boundingBoxMax.z - ray->origin.z) / ray->direction.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false; // no intersection

	if (tzmin > tmin)
		tmin = tzmin;

	// early out
	if (tmin > ray->t)
	{
		return false;
	}

	return true;
}
