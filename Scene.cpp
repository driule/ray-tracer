#include "precomp.h"


Scene::Scene(Surface* screen)
{
	this->screen = screen;
	this->camera = new Camera();

	// create scene lights
	this->lightSources = new LightSource*[1];
	this->lightSources[0] = new LightSource(vec3(20, 20, 4));

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f));
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f));

	this->primitives = new Primitive*[5];

	this->primitives[0] = new Sphere(redMaterial, vec3(0, 0, 5), 2);
	this->primitives[1] = new Sphere(redMaterial, vec3(-3.5, -3.5, 5), 1);
	this->primitives[2] = new Triangle(greenMaterial, vec3(4, 4, 5), vec3(2, 2, 5), vec3(2, 5, 5));
}

void Scene::render(int row)
{
	//for (int y = 0; y < SCRHEIGHT; y++)
	//{
		for (int x = 0; x < SCRWIDTH; x++)
		{
			Ray* ray = this->camera->generateRay(x, row);
			Pixel color = 0x048880; // background color
			bool intersected = false;
			// check intersections with primitives
			for (int i = 0; i < 3; i++) {

				if (this->primitives[i]->intersects(ray)) {
					color = (int)this->primitives[i]->material->color.z * 255 + ((int)(this->primitives[i]->material->color.y * 255) << 8) + ((int)(this->primitives[i]->material->color.x * 255) << 16);
					intersected = true;
				}
			}

			if (intersected)
			{
				Ray* intersectionRay = new Ray();
				intersectionRay->origin = ray->origin + ray->t * ray->direction;
				intersectionRay->direction = normalize(this->lightSources[0]->position - intersectionRay->origin);
				bool obstructed = false;
				for (int i = 0; i < 3; i++) {
					if (this->primitives[i]->intersects(intersectionRay)) {
						obstructed = true;
					}
				}
				if (obstructed)
				{
					color = 0x000000;
				}
				delete intersectionRay;
			}

			// plot pixel with color
			this->screen->Plot(x, row, color);

			// clear garbages
			delete ray;
		//}
	}
}
