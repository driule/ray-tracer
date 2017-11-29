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
		Primitive(Material* material);
		
		Material* material;

		virtual bool intersects(Ray* ray) = 0;
	};

	class Sphere : public Primitive
	{
	public:
		Sphere(Material* material, vec3 position, int radius);

		vec3 position;
		int radius, radius2;

		bool intersects(Ray* ray);
	};

	class Triangle : public Primitive
	{
	public:
		Triangle(Material* material, vec3 a, vec3 b, vec3 c);

		vec3 a, b, c;

		bool intersects(Ray* ray);
	};
}
