#pragma once
namespace Tmpl8
{
	class BVH
	{
	public:
		BVH(int id, std::vector<Primitive*> primitives);

		BVHNode* root;
		int id;
		int* primitiveIndices;

		void createBVH(int startIndex, int endIndex);
		
	private:
		std::vector<Primitive*> primitives;

		void calculateBounds(BVHNode* node);
		void subdivide(BVHNode* node, int depth);
		void partition(BVHNode* node);
	};
}
