#include "precomp.h"


Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::render()
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
