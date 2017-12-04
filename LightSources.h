#pragma once
namespace Tmpl8 {
	class LightSource
	{
	public:
		LightSource(int id, vec3 position, vec4 color);

		vec3 position;
		vec4 color;

		virtual void intersect(Ray* ray) = 0;
	protected:
		int id;
	};

	class DirectLight : public LightSource
	{
	public:
		DirectLight(int id, vec3 position, vec4 color);

		void intersect(Ray* ray);
	};
}

