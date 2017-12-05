#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);
		Camera* camera;

		void render(int row);
		void loadObjModel(const char *filename, Material* material);
	private:
		Surface* screen;

		std::vector<Primitive*> primitives;
		std::vector<LightSource*> lightSources;

		vec4 trace(Ray* ray, int depth);
		vec4 DirectIllumination(Ray* ray);
		void intersectPrimitives(Ray* ray);
		void intersectLightSources(Ray* ray);
		Pixel convertColorToPixel(vec4 color);
	};
}
