#pragma once
namespace Tmpl8
{
	class TopBVH : public BVH
	{
	public:
		TopBVH();
		TopBVH(std::vector<Primitive*> primitives, std::vector<BVH*> BVHs);
		~TopBVH();

		BVHNode* root;

		void traverse(BVHNode* node, Ray* ray, bool isShadowRay);

	protected:
		void subdivide(BVHNode* node);

	private:
		int* primitiveIndices;
		std::vector<BVH*> BVHs;
		std::vector<BVHNode*> nodes;
	};
}

