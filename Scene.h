#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);

		Primitive** primitives;
		LightSource** lightSources;
		Camera* camera;
		vec3 lightSource;
		Surface* screen;

		void render(int row);
	};
}
