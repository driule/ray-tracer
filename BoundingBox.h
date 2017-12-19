#pragma once
namespace Tmpl8
{
	class BoundingBox
	{
	public:
		BoundingBox();
		BoundingBox(vec3 min, vec3 max);

		vec3 min, max, center;

		void calculateCenter();
		void translate(vec3 vector);
		float calculateSurfaceArea();
	};
}
