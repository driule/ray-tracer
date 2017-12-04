#include "precomp.h"

Scene::Scene(Surface* screen)
{
	// create camera
	this->screen = screen;
	this->camera = new Camera();

	// create scene lights
	this->lightSourcesCount = 1;
	this->lightSources = new LightSource*[lightSourcesCount];
	this->lightSources[0] = new LightSource(vec3(10, 0, 1), 0);

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f), diffuse);
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f), diffuse);
	Material* blueMaterial = new Material(vec4(0.0f, 0.0f, 1.0f, 0.0f), diffuse);
	Material* planeMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), diffuse);

	this->primitivesCount = 4;
	this->primitives = new Primitive*[this->primitivesCount];

	this->primitives[0] = new Sphere(blueMaterial, 0, vec3(0, 0, 5), 1);
	this->primitives[1] = new Sphere(redMaterial, 1, vec3(0, 0, 10), 4);
	this->primitives[2] = new Triangle(greenMaterial, 2, vec3(4, 4, 5), vec3(2, 2, 5), vec3(2, 5, 5));

	this->primitives[3] = new Plane(planeMaterial, 3, vec3(0, 0, 15), vec3(0, 0, 1));
}

void Scene::render(int row)
{
	for (int x = 0; x < SCRWIDTH; x++)
	{
		// generate and trace ray
		Ray* ray = this->camera->generateRay(x, row);
		vec4 color = this->trace(ray, 0);

		// plot pixel with color
		this->screen->Plot(x, row, this->convertColorToPixel(color));

		// clear garbages
		delete ray;
	}
}

vec4 Scene::trace(Ray* ray, int depth)
{
	depth += 1;
	if (depth > 5) return BGCOLOR;
	this->intersectPrimitives(ray);

	if (ray->intersectedObjectId == -1) // no primitive intersected
	{
		this->intersectLightSources(ray);
		if (ray->intersectedObjectId == -1)
		{
			return BGCOLOR;
		}
		else
		{
			// direct light
			return vec4(1, 1, 1, 1);
		}
	}

	// primitive intersected
	Material* material = this->primitives[ray->intersectedObjectId]->material;
	if (material->type == diffuse)
	{
		Ray* reflectionRay = new Ray();
		reflectionRay->origin = ray->origin + ray->t * ray->direction;
		reflectionRay->direction = normalize(this->lightSources[0]->position - reflectionRay->origin);

		this->intersectPrimitives(reflectionRay);
		if (reflectionRay->intersectedObjectId == -1)
		{
			delete reflectionRay;
			return material->color;
		}
		else
		{
			// cast shadow
			delete reflectionRay;
			return vec4(material->color.x * 0.5, material->color.y * 0.5, material->color.z * 0.5, material->color.w * 0.5);
		}
	}

	return BGCOLOR;
}

void Scene::intersectPrimitives(Ray* ray)
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

Pixel Scene::convertColorToPixel(vec4 color)
{
	return (int)(color.z * 255) + ((int)(color.y * 255) << 8) + ((int)(color.x * 255) << 16);
}