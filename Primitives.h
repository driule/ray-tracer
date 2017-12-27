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
		Primitive(Material* material);
		
		int id;
		Material* material;
		BoundingBox* boundingBox;

		virtual void intersect(Ray* ray) = 0;
		virtual vec3 getNormal(vec3 point) = 0;
		virtual void translate(vec3 vector) = 0;
	};

	class Sphere : public Primitive
	{
	public:
		Sphere(Material* material, vec3 position, float radius);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);
		void translate(vec3 vector);

	private:
		vec3 position;
		float radius, radius2;
	};

	class Triangle : public Primitive
	{
	public:
		Triangle(Material* material, vec3 a, vec3 b, vec3 c);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);
		void translate(vec3 vector);

	private:
		vec3 a, b, c;
		vec3 normal;
	};

	class Plane : public Primitive
	{
	public:
		Plane(Material* material, vec3 position, vec3 direction, float size = 10);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);
		void translate(vec3 vector);

	private:
		vec3 position, direction;
	};

	class Cylinder : public Primitive
	{
	public:
		Cylinder(Material* material, vec3 position, vec3 upVector, float radius, float height);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);
		void translate(vec3 vector);

	private:
		vec3 position, upVector;
		float radius, height;
	};

	class Torus : public Primitive
	{
	public:
		Torus(Material* material, float R, float r, vec3 position, vec3 axis);

		void intersect(Ray* ray);
		vec3 getNormal(vec3 point);
		void translate(vec3 vector);

	private:
		float R, r;
		vec3 position, axis;

	};
}
