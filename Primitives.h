#pragma once
namespace Tmpl8 {
	class Material
	{
	public:
		Material(vec4 color, MaterialType type);

		vec4 color;
		float reflection, refraction;
		MaterialType type;
	};

	class Primitive
	{
	public:
		Primitive(Material* material, int id);
		
		Material* material;
		int id;

		virtual void intersect(Ray* ray) = 0;
		virtual vec3 getNormal(vec3 point) = 0;
	};

	class Sphere : public Primitive
	{
	public:
		Sphere(Material* material, int id, vec3 position, float radius);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);

	private:
		vec3 position;
		float radius, radius2;
	};

	class Triangle : public Primitive
	{
	public:
		Triangle(Material* material, int id, vec3 a, vec3 b, vec3 c);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);

	private:
		vec3 a, b, c;
	};

	class Plane : public Primitive
	{
	public:
		Plane(Material* material, int id, vec3 position, vec3 direction);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);

	private:
		vec3 position, direction;
	};

	class Cylinder : public Primitive
	{
	public:
		Cylinder(Material* material, int id, vec3 position, float radius, float height);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);

	private:
		vec3 position;
		float radius, height;
	};
}
