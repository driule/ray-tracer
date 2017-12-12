#pragma once
namespace Tmpl8
{
	class BVH
	{
	public:
		BVH(std::vector<Primitive*> primitives);

		struct Node
		{
			vec3 boundingBoxMin, boundingBoxMax;
			bool isLeaf = false;
			Node *left, *right;
			int first, count;
		};

		Node* root;
		void traverse(Node* node, Ray* ray);
	private:
		std::vector<Primitive*> primitives;
		int* primitiveIndices;

		void subdivide(Node* node);
		void calculateBounds(Node* node, int first, int count);
		void partition(Node* node);
		bool intersects(Node* node, Ray* ray);
	};
}
