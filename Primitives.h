#pragma once
namespace Tmpl8 {
	class Material
	{
	public:
		vec4 color;
	};

	class Primitive
	{
	public:
		Primitive();
		vec3 position;
		Material material;
	};

	class Sphere: public Primitive
	{
	public:
		Sphere(vec3 position, int radius);
		int radius;
	};
}
