#include "precomp.h"


Scene::Scene()
{
	this->camera = new Camera();
}

void Scene::render(Surface* screen)
{
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		for (int x = 0; x < SCRWIDTH; x++)
		{
			vec3 ray = this->camera->generateRay(x, y);

			// TODO: ray tracing
			// it returns one color for a ray
		}
	}
}
