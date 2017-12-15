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
		void traverse(Node* node, Ray* ray, bool isShadowRay = false);
	private:
		std::vector<Primitive*> primitives;
		int* primitiveIndices;

		void subdivide(Node* node, int depth);
		void calculateBounds(Node* node);
		void partition(Node* node);
		void randomPartition(Node* node);
		void binnedPartition(Node* node);
		float calculateSurfaceArea(Node* node);
		bool intersects(Node* node, Ray* ray);
	};
}
