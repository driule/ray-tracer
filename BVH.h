#pragma once
namespace Tmpl8
{
	class BVH
	{
	public:
		BVH(std::vector<Primitive*> primitives);

		BVHNode* root;
		int id;
		int* objectIndices;

		void createBVH(int id, int startIndex, int endIndex);

	protected:
		std::vector<Primitive*> primitives;
		std::vector<BoundingBox*> boundingBoxes;

		void calculateBounds(BVHNode* node);
		void subdivide(BVHNode* node, int depth);
		void partition(BVHNode* node, int binCount);
	};
}
