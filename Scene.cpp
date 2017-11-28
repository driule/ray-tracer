#include "precomp.h"


Scene::Scene()
{
	this->camera = new Camera();

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f));
	this->primitives = new Sphere*[5];
	this->primitives[0] = new Sphere(redMaterial, vec3(0, 5, 5), 1);
	this->primitives[1] = new Sphere(redMaterial, vec3(5, 0, 5), 2);
	this->primitives[2] = new Sphere(redMaterial, vec3(5, 5, 5), 1);
	this->primitives[3] = new Sphere(redMaterial, vec3(10, 5, 5), 2);
	this->primitives[4] = new Sphere(redMaterial, vec3(10, 10, 5), 1);
}

void Scene::render(Surface* screen)
{
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		for (int x = 0; x < SCRWIDTH; x++)
		{
			Ray* ray = this->camera->generateRay(x, y);
			Pixel color = 0x048880; // background color

			// check intersections with spheres
			for (int i = 0; i < 5; i++) {

				vec3 c = this->primitives[i]->position - this->camera->position;
				float t = dot(c, ray->direction);
				vec3 q = c - t * ray->direction;
				float p2 = dot(q, q);
				if (p2 > this->primitives[i]->radius * this->primitives[i]->radius) continue;
				t = sqrt(this->primitives[i]->radius * this->primitives[i]->radius - p2);

				if ((t < ray->t) && (t > 0)) {
					ray->t = t;

					//Pixel color = (int)this->primitives[i]->material->color.x * 255 + ((int)(this->primitives[i]->material->color.y * 255) << 8) + ((int)(this->primitives[i]->material->color.z * 255) << 16);
					color = 0x00ff00;
				}
			}

			// plot pixel with color
			screen->Plot(x, y, color);
		}
	}
}
