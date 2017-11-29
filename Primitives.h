#pragma once
namespace Tmpl8 {
	class Material
	{
	public:
		Material(vec4 color);
		vec4 color;
	};

	class Primitive
	{
	public:
		Primitive();
		vec3 position;
		Material* material;

		virtual bool intersects(Ray* ray) = 0;
	};

	class Sphere : public Primitive
	{
	public:
		Sphere(Material* material, vec3 position, int radius);
		int radius;

		bool intersects(Ray* ray);
	};

	class Triangle : public Primitive
	{
	public:
		Triangle(Material* material, vec3 v1, vec3 v2, vec3 v3);
		vec3 v1, v2, v3;

		bool intersects(Ray* ray);
	};
}
