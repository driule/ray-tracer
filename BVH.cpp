#include "precomp.h"

BVH::BVH(std::vector<Primitive>* primitives)
{
	this->primitives = primitives;
	int N = primitives->size();
	this->primitiveIndices = new uint[N];
	for (uint i = 0; i < N; i++)
	{
		primitiveIndices[i] = i;
	}

	this->root->first = 0;
	this->root->count = N;
	calculateBounds(this->root);
	subdivide(this->root);
}

void BVH::subdivide(Node* node)
{
	if (node->count < 3)
	{
		node->isLeaf = true;
		return;
	}
	node->left = new Node();
	node->right = new Node();
	this->partition(node);
	//this->subdivide(node->left);
	//this->subdivide(node->right);
}

void BVH::calculateBounds(Node* node)
{
	//uint idx = this->primitiveIndices[node->first];
	//node->leftTopFront = this->primitives->at(idx).center;
	node->leftTopFront = vec3(-10, -10, -10);
	node->rightBottomBack = vec3(10, 10, 10);
}

void BVH::partition(Node* node)
{
	// splitting the primitives correctly
	// left Node
	node->left->first = node->first;
	node->left->count = node->count / 2;

	calculateBounds(node->left);

	// right Node
	node->right->first = node->left->first + node->left->count;
	node->right->count = node->count - node->left->count;

	calculateBounds(node->right);
}

void BVH::traverse(Node* node, Ray* ray)
{
	// if(!ray intersects node) return;
	if (node->isLeaf)
	{
		// intersect primitves
	}
	else
	{
		this->traverse(node->left, ray);
		this->traverse(node->right, ray);
	}
}


