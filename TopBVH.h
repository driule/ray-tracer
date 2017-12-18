#pragma once
namespace Tmpl8
{
	class TopBVH
	{
	public:
		TopBVH(std::vector<Primitive*> primitives, std::vector<BVH*> BVHs);

		BVHNode* root;

		void traverse(BVHNode* node, Ray* ray, bool isShadowRay);
	private:
		std::vector<Primitive*> primitives;
		int* primitiveIndices;

		std::vector<BVH*> BVHs;
		int* BVHsIndices;

		void calculateBounds(BVHNode* node);
		void subdivide(BVHNode* node);
		void partition(BVHNode* node);
	};
}

