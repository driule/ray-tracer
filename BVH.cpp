#include "precomp.h"

#define MAX_PRIMITIVES 4
#define MAX_DEPTH 100

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

	calculateBounds(this->root, 0, N);
	subdivide(this->root, 0);
}

void BVH::subdivide(Node* node, int depth)
{
	if (node->count <= MAX_PRIMITIVES || depth >= MAX_DEPTH)
	{
		node->isLeaf = true;
		return;
	}

	node->left = new Node();
	node->right = new Node();
	this->partition(node);

	depth++;
	this->subdivide(node->left, depth);
	this->subdivide(node->right, depth);
}

void BVH::calculateBounds(Node* node, int first, int count)
{
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;
	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	
	for (int i = first; i < first + count; i++)
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
	vec3 splitPlane = node->boundingBoxMin + 0.5 * node->boundingBoxMax;
	float optimalSAH = INFINITY;

	for (int j = 0; j < 3; j++)
	{
		int* temporaryIndices = new int[node->count];
		int leftCount = 0, rightCount = 0;

		for (int i = node->first; i < node->first + node->count; i++)
		{
			int index = this->primitiveIndices[i];

			bool left = false;
			if (j == 0)
			{
				left = this->primitives[index]->center.x < splitPlane.x;
			}
			else if (j == 1)
			{
				left = this->primitives[index]->center.y < splitPlane.y;
			}
			else
			{
				left = this->primitives[index]->center.z < splitPlane.z;
			}

			if (left)
			{
				temporaryIndices[leftCount] = index;
				leftCount++;
			}
			else
			{
				rightCount++;
				temporaryIndices[node->count - rightCount] = index;
			}
		}

		// left Node
		calculateBounds(node->left, node->first, leftCount);

		// right Node
		calculateBounds(node->right, node->first + leftCount, rightCount);

		vec3 diagonalLeft = (node->left->boundingBoxMax - node->left->boundingBoxMin);
		vec3 diagonalRight = (node->right->boundingBoxMax - node->right->boundingBoxMin);

		float surfaceAreaLeft = (abs(diagonalLeft.x * diagonalLeft.y) + abs(diagonalLeft.x * diagonalLeft.z) + abs(diagonalLeft.z * diagonalLeft.y)) * 2;
		float surfaceAreaRight = (abs(diagonalRight.x * diagonalRight.y) + abs(diagonalRight.x * diagonalRight.z) + abs(diagonalRight.z * diagonalRight.y)) * 2;

		float SAH = surfaceAreaLeft * leftCount + surfaceAreaRight * rightCount;

		if (SAH < optimalSAH)
		{
			optimalSAH = SAH;
			node->left->first = node->first;
			node->left->count = leftCount;

			node->right->first = node->first + leftCount;
			node->right->count = rightCount;
			for (int i = 0; i < node->count; i++)
			{
				this->primitiveIndices[node->first + i] = temporaryIndices[i];
			}
		}

		delete temporaryIndices;
	}
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

	return true;
}


