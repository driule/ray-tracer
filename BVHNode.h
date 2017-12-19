#pragma once
namespace Tmpl8
{
	class BVHNode
	{
	public:
		BVHNode();

		BoundingBox* boundingBox;
		bool isLeaf;
		BVHNode *left, *right;
		int first, count;

		bool intersects(Ray* ray);
		void translate(vec3 vector);
	private:
	};
}

