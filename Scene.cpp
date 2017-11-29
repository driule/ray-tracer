#include "precomp.h"


Scene::Scene()
{
	this->camera = new Camera();

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f));
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f));

	this->primitives = new Primitive*[5];

	this->primitives[0] = new Sphere(redMaterial, vec3(0, 0, 5), 2);
	this->primitives[1] = new Sphere(redMaterial, vec3(-3.5, -3.5, 5), 1);
	this->primitives[2] = new Triangle(greenMaterial, vec3(4, 4, 5), vec3(2, 2, 5), vec3(2, 5, 5));
}

void Scene::render(Surface* screen)
{
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		for (int x = 0; x < SCRWIDTH; x++)
		{
			Ray* ray = this->camera->generateRay(x, y);
			Pixel color = 0x048880; // background color

			// check intersections with primitives
			for (int i = 0; i < 3; i++) {

				if (this->primitives[i]->intersects(ray)) {
					color = (int)this->primitives[i]->material->color.z * 255 + ((int)(this->primitives[i]->material->color.y * 255) << 8) + ((int)(this->primitives[i]->material->color.x * 255) << 16);
				}
			}

			// plot pixel with color
			screen->Plot(x, y, color);

			// clear garbages
			delete ray;
		}
	}
}
