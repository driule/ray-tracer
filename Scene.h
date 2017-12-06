#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);
		Camera* camera;

		void render(int row);
		void loadObjModel(const char *filename, Material* material);

		std::vector<Primitive*> primitives;
		std::vector<LightSource*> lightSources;
	private:
		Surface* screen;

		vec4 trace(Ray* ray, int depth);
		vec4 illuminate(Ray* ray);
		Ray* computeReflectionRay(Ray* ray);
		Ray* computeRefractionRay(Ray* ray);
		void intersectPrimitives(Ray* ray);
		void intersectLightSources(Ray* ray);
		Pixel convertColorToPixel(vec4 color);
	};
}
