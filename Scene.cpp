#include "precomp.h"


Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::render()
{
	this->camera->generateRay(0,0);
}
