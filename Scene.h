#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene();

		//Primitive** primitives;
		Sphere** primitives;
		Camera* camera;
		vec3 lightSource;

		void render(Surface* screen);
	};
}
