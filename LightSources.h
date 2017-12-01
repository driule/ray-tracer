#pragma once
namespace Tmpl8 {
	class LightSource
	{
	public:
		LightSource(vec3 position, int id);
		vec3 position;
		void intersect(Ray* ray);
		int id;
	};
}

