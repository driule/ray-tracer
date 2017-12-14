#pragma once
namespace Tmpl8 {
	class Scene
	{
	public:
		Scene(Surface* screen);
		Camera* camera;

		void render(int row);
		void addPrimitive(Primitive* primitive);
		void addLightSource(LightSource* lightSource);

		// returns loaded model id
		int loadModel(const char *filename, Material* material, vec3 translationVector = vec3(0));
		void translateModel(int id, vec3 vector);

		void clear();
	private:
		Surface* screen;
		BVH* accelerationStructure;

		std::vector<Primitive*> primitives;
		std::vector<LightSource*> lightSources;

		struct Model
		{
			Model::Model(int id, int startIndex, int endIndex)
			{
				this->id = id;
				this->startIndex = startIndex;
				this->endIndex = endIndex;
			}
			int id, startIndex, endIndex;
		};
		std::vector<Model*> models;

		vec4 trace(Ray* ray, int depth);
		vec4 illuminate(Ray* ray);
		Ray* computeReflectionRay(Ray* ray);
		Ray* computeRefractionRay(Ray* ray);
		void intersectPrimitives(Ray* ray, bool isShadowRay = false);
		void intersectLightSources(Ray* ray);

		Pixel convertColorToPixel(vec4 color);

		void createModel(int id, int startIndex, int endIndex);

		void createBVH();
	};
}
