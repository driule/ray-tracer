#pragma once

namespace Tmpl8 {
	class Ray
	{
	public:
		Ray();
		Ray(vec3 origin, vec3 direction, float t);

		vec3 origin;
		vec3 direction;
		float t;
	};
}

