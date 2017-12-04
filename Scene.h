#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);
		Camera* camera;

		void render(int row);
	private:
		Surface* screen;

		int primitivesCount;
		Primitive** primitives;

		int lightSourcesCount;
		LightSource** lightSources;

		vec4 trace(Ray* ray, int depth);
		vec4 DirectIllumination(vec3 intersection, vec3 normal);
		void intersectPrimitives(Ray* ray);
		void intersectLightSources(Ray* ray);
		Pixel convertColorToPixel(vec4 color);
	};
}
