#pragma once
namespace Tmpl8
{
	class BVH
	{
	public:
		BVH(int id, std::vector<Primitive*> primitives);
		~BVH();

		BVHNode* root;
		int id;
		int* primitiveIndices;

		void createBVH(int startIndex, int endIndex);
		
	private:
		std::vector<Primitive*> primitives;

		void subdivide(BVHNode* node, int depth);
		void calculateBounds(BVHNode* node);
		void partition(BVHNode* node);
	};
}
