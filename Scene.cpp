#include "precomp.h"

Scene::Scene(Surface* screen)
{
	this->screen = screen;
	this->camera = new Camera();

	// create scene lights
	this->lightSourcesCount = 1;
	this->lightSources = new LightSource*[lightSourcesCount];
	this->lightSources[0] = new LightSource(vec3(10, 0, 1), 0);

	// create scene objects
	//Material* lightMaterial = new Material(vec4(1.0f, 1.0f, 1.0f, 1.0f), emissive);
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f), diffuse);
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f), diffuse);
	Material* blueMaterial = new Material(vec4(0.0f, 0.0f, 1.0f, 0.0f), diffuse);

	this->primitivesCount = 3;
	this->primitives = new Primitive*[this->primitivesCount];
	

	this->primitives[0] = new Sphere(blueMaterial, 0, vec3(0, 0, 5), 1);
	this->primitives[1] = new Sphere(redMaterial, 1, vec3(0, 0, 10), 4);
	this->primitives[2] = new Triangle(greenMaterial, 2, vec3(4, 4, 5), vec3(2, 2, 5), vec3(2, 5, 5));
	//this->primitives[3] = new Sphere(lightMaterial, 3, vec3(10, 0, 1), 1);
}

void Scene::render(int row)
{
	for (int x = 0; x < SCRWIDTH; x++)
	{
		Ray* ray = this->camera->generateRay(x, row);
		Pixel pixelColor = 0x048880; // background color

		// check intersections with primitives
		/*bool intersected = false;
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
				if (this->primitives[i]->intersects(intersectionRay)) {
					color = 0x000000; //cast shadow
					break;
				}
			}

			delete intersectionRay;
		}*/
		vec4 color = this->trace(ray, 0);
		pixelColor = (int)(color.z * 255) + ((int)(color.y * 255) << 8) + ((int)(color.x * 255) << 16);


		// plot pixel with color
		this->screen->Plot(x, row, pixelColor);

		// clear garbages
		delete ray;
	}
}

vec4 Scene::trace(Ray* ray, int depth)
{
	depth += 1;
	if (depth > 5) return BGCOLOR;
	this->intersect(ray);

	if (ray->intersectedPrimitiveId == -1)
	{
		this->intersectLightSources(ray);
		if (ray->intersectedPrimitiveId == -1)
		{
			return BGCOLOR;
		}
		else
		{
			return vec4(1, 1, 1, 1);
		}
	}

	// All primitives are diffuse
	Material* material = this->primitives[ray->intersectedPrimitiveId]->material;
	if (material->type == diffuse)
	{
		Ray* reflectionRay = new Ray();
		reflectionRay->origin = ray->origin + ray->t * ray->direction;
		reflectionRay->direction = normalize(this->lightSources[0]->position - reflectionRay->origin);

		this->intersect(reflectionRay);
		if (reflectionRay->intersectedPrimitiveId == -1)
		{
			delete reflectionRay;
			return material->color;
		}
		else
		{
			delete reflectionRay;
			return vec4(0, 0, 0, 1);
		}
	}

	return BGCOLOR;
}

void Scene::intersect(Ray* ray)
{
	for (int i = 0; i < this->primitivesCount; i++)
	{
		this->primitives[i]->intersect(ray);
	}
}

void Scene::intersectLightSources(Ray* ray)
{
	for (int i = 0; i < this->lightSourcesCount; i++)
	{
		this->lightSources[i]->intersect(ray);
	}
}
