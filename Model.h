#pragma once
namespace Tmpl8 {
	class Model
	{
	public:
		Model(std::vector<Primitive*> primitives);
		void translate(vec3 vector);

		std::vector<Primitive*> primitives;
	};
}

