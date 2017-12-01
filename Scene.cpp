#include "precomp.h"

Scene::Scene(Surface* screen)
{
	this->screen = screen;
	this->camera = new Camera();

	// create scene lights
	this->lightSources = new LightSource*[1];
	this->lightSources[0] = new LightSource(vec3(10, 0, 1));

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f));
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f));
	Material* blueMaterial = new Material(vec4(0.0f, 0.0f, 1.0f, 0.0f));

	this->primitives = new Primitive*[5];

	this->primitives[0] = new Sphere(blueMaterial, vec3(0, 0, 5), 1);
	this->primitives[1] = new Sphere(redMaterial, vec3(0, 0, 10), 6);
	this->primitives[2] = new Triangle(greenMaterial, vec3(4, 4, 5), vec3(2, 2, 5), vec3(2, 5, 5));
}

void Scene::render(int row)
{
	for (int x = 0; x < SCRWIDTH; x++)
	{
		Ray* ray = this->camera->generateRay(x, row);
		Pixel color = 0x048880; // background color

		// check intersections with primitives
		bool intersected = false;
		int intersectedIndex;

		for (int i = 0; i < 3; i++) {
			if (this->primitives[i]->intersects(ray)) {
				intersected = true;
				intersectedIndex = i;
			}
		}

		if (intersected)
		{
			color = (int)this->primitives[intersectedIndex]->material->color.z * 255 + ((int)(this->primitives[intersectedIndex]->material->color.y * 255) << 8) + ((int)(this->primitives[intersectedIndex]->material->color.x * 255) << 16);

			Ray* intersectionRay = new Ray();
			intersectionRay->origin = ray->origin + ray->t * ray->direction;
			intersectionRay->direction = normalize(this->lightSources[0]->position - intersectionRay->origin);

			for (int i = 0; i < 3; i++) {
				if (intersectedIndex == i) {
					continue;
				}
				if (this->primitives[i]->intersects(intersectionRay)) {
					color = 0x000000; //cast shadow
					break;
				}
			}

			delete intersectionRay;
		}

		// plot pixel with color
		this->screen->Plot(x, row, color);

		// clear garbages
		delete ray;
	}
}
