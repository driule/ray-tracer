#pragma once
namespace Tmpl8 {
	class LightSource
	{
	public:
		LightSource(vec3 position, vec4 color, int intensity);

		int id;
		vec3 position;
		vec4 color;
		int intensity;

		virtual void intersect(Ray* ray) = 0;
	};

	class DirectLight : public LightSource
	{
	public:
		DirectLight(vec3 position, vec4 color, int intensity);

		void intersect(Ray* ray);
	};
}

