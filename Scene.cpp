#include "precomp.h"

Scene::Scene(Surface* screen)
{
	// create camera
	this->screen = screen;
	this->camera = new Camera();
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
			return this->lightSources[ray->intersectedObjectId]->color;
		}
	}

	// primitive intersected
	Material* material = this->primitives[ray->intersectedObjectId]->material;
	vec4 color = (material->color * 0.2);
	if (material->type == diffuse)
	{
		return color + material->color * illuminate(ray);
	}
	if (material->type == mirror)
	{
		Ray* reflectionRay = computeReflectionRay(ray);
		vec4 reflectionColor = this->trace(reflectionRay, depth);
		delete reflectionRay;

		return color + reflectionColor * 0.8;
	}
	if (material->type == dielectric)
	{
		Ray* refractionRay = this->computeRefractionRay(ray);
		if (refractionRay->intersectedObjectId == -2)
		{
			delete refractionRay;
			return color;
		}
		else
		{
			color += this->trace(refractionRay, depth);
		}

		Ray* reflectionRay = this->computeReflectionRay(ray);
		vec4 reflectionColor = this->trace(reflectionRay, depth);

		delete refractionRay;
		delete reflectionRay;

		return material->reflection * reflectionColor + material->color.w * color;
	}

	return BGCOLOR;
}

vec4 Scene::illuminate(Ray* ray)
{
	vec4 color = vec4(0, 0, 0, 0);
	
	for (int i = 0; i < this->lightSources.size(); i++)
	{
		Ray* shadowRay = new Ray();
		shadowRay->origin = ray->origin + ray->t * ray->direction;
		shadowRay->direction = normalize(this->lightSources[i]->position - shadowRay->origin);

		vec3 normal = this->primitives[ray->intersectedObjectId]->getNormal(shadowRay->origin);

		float dotDirectionNormal = dot(shadowRay->direction, normal);
		if (dotDirectionNormal < 0)
		{
			delete shadowRay;
			continue;
		}

		vec3 hitEpsilon = shadowRay->origin + shadowRay->direction * 0.01;
		shadowRay->origin = hitEpsilon;
		shadowRay->t = (shadowRay->origin - this->lightSources[i]->position).length();
		this->intersectPrimitives(shadowRay, true);

		if (shadowRay->intersectedObjectId != -1)
		{
			delete shadowRay;
			continue;
		}

		float attenuation = this->lightSources[i]->intensity / (this->lightSources[i]->position - shadowRay->origin).sqrLentgh();
		color += dotDirectionNormal * attenuation * this->lightSources[i]->color;
		delete shadowRay;
	}

	return color;
}

Ray* Scene::computeReflectionRay(Ray* ray)
{
	vec3 hitPoint = ray->origin + ray->t * ray->direction;
	vec3 N = this->primitives[ray->intersectedObjectId]->getNormal(hitPoint);

	Ray* reflectionRay = new Ray();
	reflectionRay->direction = ray->direction - 2 * (ray->direction * N) * N;
	reflectionRay->origin = hitPoint + reflectionRay->direction * EPSILON;

	return reflectionRay;
}

Ray* Scene::computeRefractionRay(Ray* ray)
{
	// source: https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel

	vec3 hitPoint = ray->origin + ray->t * ray->direction;
	vec3 N = this->primitives[ray->intersectedObjectId]->getNormal(hitPoint);
	float incommingAngle = dot(N, ray->direction);

	float cosi = CLAMP(-1, 1, incommingAngle);
	float etai = 1, etat = this->primitives[ray->intersectedObjectId]->material->refraction;
	vec3 n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);

	Ray* refractionRay = new Ray();
	if (k < 0)
	{
		refractionRay->intersectedObjectId = -2;
		return refractionRay;
	}
	else
	{
		refractionRay->direction = eta * ray->direction + (eta * cosi - sqrtf(k)) * n;
		refractionRay->origin = hitPoint + refractionRay->direction * 0.01;
		
		return refractionRay;
	}
}

void Scene::intersectPrimitives(Ray* ray, bool isShadowRay)
{
	if (BVH_ENABLED)
	{
		this->topBHV->traverse(this->topBHV->root, ray, isShadowRay);
	}
	else
	{
		for (int i = 0; i < this->primitives.size(); i++)
		{
			this->primitives[i]->intersect(ray);
			//this->primitives[i]->intersectBoundingBox(ray);
		}
	}
}

void Scene::intersectLightSources(Ray* ray)
{
	for (int i = 0; i < this->lightSources.size(); i++)
	{
		this->lightSources[i]->intersect(ray);
	}
}

Pixel Scene::convertColorToPixel(vec4 color)
{
	color *= 255.0f;
	int r = min((int)color.x, 255);
	int g = min((int)color.y, 255);
	int b = min((int)color.z, 255);

	return (r << 16) + (g << 8) + b;
}

void Scene::createTopBVH()
{
	//if (this->topBHV != NULL && this->topBHV->root != NULL)
		//delete this->topBHV;

	this->topBHV = new TopBVH(this->primitives, BVHs);
}

void Scene::addPrimitive(Primitive* primitive)
{
	primitive->id = this->primitives.size();
	this->primitives.push_back(primitive);
	this->createTopBVH();
}

void Scene::addLightSource(LightSource* lightSource)
{
	lightSource->id = this->lightSources.size();
	this->lightSources.push_back(lightSource);
}

void Scene::clear()
{
	this->primitives.clear();
	this->lightSources.clear();
	this->models.clear();
}

int Scene::loadModel(const char *filename, Material* material, vec3 translationVector)
{
	// obj file content
	std::vector<vec3> vertices;
	std::vector<int> faceIndexes;
	std::vector<vec3> meshVertices;

	std::ifstream stream(filename, std::ios::in);
	if (!stream)
	{
		printf("Cannot load %s file!", filename);
	}

	std::string line;
	while (std::getline(stream, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream v(line.substr(2));
			float x, y, z;
			v >> x; v >> y; v >> z;

			vertices.push_back(vec3(x, y, z));
		}
		else if (line.substr(0, 2) == "vt")
		{
			// TODO: implement textures support
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream v(line.substr(2));
			int a, b, c;
			v >> a; v >> b; v >> c;

			faceIndexes.push_back(--a);
			faceIndexes.push_back(--b);
			faceIndexes.push_back(--c);
		}
	}

	// calculate mesh vertices
	for (unsigned int i = 0; i < faceIndexes.size(); i++)
	{
		meshVertices.push_back(
			vec3(vertices[faceIndexes[i]].x, vertices[faceIndexes[i]].y, vertices[faceIndexes[i]].z) + translationVector
		);
	}

	// add triangles to the scene
	int primitivesCount = meshVertices.size() / 3;
	printf("primitives count in %s file: %i\n", filename, primitivesCount);

	int startIndex = this->primitives.size();

	for (int i = 0; i < primitivesCount; i++)
	{
		vec3 a = meshVertices[i * 3];
		vec3 b = meshVertices[i * 3 + 1];
		vec3 c = meshVertices[i * 3 + 2];

		Triangle* triangle = new Triangle(material, a, b, c);
		triangle->id = this->primitives.size();
		this->primitives.push_back(triangle);
	}

	// create model
	int endIndex = this->primitives.size() - 1;
	int modelId = this->models.size();
	this->models.push_back(
		new Model(modelId, startIndex, endIndex)
	);

	// add new BVH to top-BVH
	BVH* modelBVH = new BVH(this->BVHs.size(), this->primitives);
	modelBVH->createBVH(startIndex, endIndex);
	this->BVHs.push_back(modelBVH);
	this->createTopBVH();

	return modelId;
}

void Scene::translateModel(int id, vec3 vector)
{
	Model* model = this->models[id];

	for (int i = model->startIndex; i <= model->endIndex; i++)
	{
		this->primitives[i]->translate(vector);
	}

	this->createTopBVH();
}