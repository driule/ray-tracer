#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);

		Primitive** primitives;
		int primitivesCount;
		LightSource** lightSources;
		int lightSourcesCount;
		Camera* camera;
		vec3 lightSource;
		Surface* screen;

		void render(int row);
	private:
		vec4 trace(Ray* ray, int depth);
		void intersect(Ray* ray);
		void intersectLightSources(Ray* ray);
	};
}
