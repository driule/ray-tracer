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
	};

	class Sphere: public Primitive
	{
	public:
		Sphere(Material* material, vec3 position, int radius);
		int radius;
	};
}
