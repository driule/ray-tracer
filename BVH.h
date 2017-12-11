#pragma once
namespace Tmpl8
{
	class BVH
	{
	public:
		BVH(std::vector<Primitive*> primitives);

		struct Node
		{
			vec3 leftTopFront, rightBottomBack;
			bool isLeaf = false;
			Node *left, *right;
			int first, count;
		};

		Node* root;
		void traverse(Node* node, Ray* ray);
	private:
		std::vector<Primitive*> primitives;
		uint* primitiveIndices;

		void subdivide(Node* node);
		void calculateBounds(Node* node);
		void partition(Node* node);
	};
}
