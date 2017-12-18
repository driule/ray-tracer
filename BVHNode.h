#pragma once
namespace Tmpl8
{
	class BVHNode
	{
	public:
		BVHNode();

		vec3 boundingBoxMin, boundingBoxMax;
		bool isLeaf;
		BVHNode *left, *right;
		int first, count;

		bool intersects(Ray* ray);
		float calculateSurfaceArea();
		void translate(vec3 vector);
		void destroy();
	private:
	};
}

