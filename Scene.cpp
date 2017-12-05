#include "precomp.h"

Scene::Scene(Surface* screen)
{
	// create camera
	this->screen = screen;
	this->camera = new Camera();

	// create scene lights
	this->lightSources.push_back(new DirectLight(0, vec3(10, -10, -20), vec4(1, 1, 1, 1)));

	// create scene objects
	Material* redMaterial = new Material(vec4(1.0f, 0.0f, 0.0f, 0.0f), diffuse);
	Material* greenMaterial = new Material(vec4(0.0f, 1.0f, 0.0f, 0.0f), diffuse);
	Material* blueGlassMaterial = new Material(vec4(0.0f, 0.0f, 1.0f, 0.0f), dielectric);
	blueGlassMaterial->refraction = 1.33;
	blueGlassMaterial->reflection = 0.5;
	Material* planeMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), diffuse);

	this->primitives.push_back(
		new Sphere(blueGlassMaterial, 0, vec3(0, 0, 5), 1)
	);
	this->primitives.push_back(
		new Sphere(redMaterial, 1, vec3(0, 0, 10), 4)
	);
	this->primitives.push_back(
		new Triangle(greenMaterial, 2, vec3(4, 4, 5), vec3(1, 1, 5), vec3(2, 5, 5))
	);
	this->primitives.push_back(
		new Plane(planeMaterial, 3, vec3(0, 0, 20), vec3(0, 0, -1))
	);
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

		return color + reflectionColor;
	}
	if (material->type == dielectric)
	{
		// source: https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
		vec3 hitPoint = ray->origin + ray->t * ray->direction;
		vec3 N = this->primitives[ray->intersectedObjectId]->getNormal(hitPoint);
		float incommingAngle = dot(N, ray->direction);

		float cosi = CLAMP(-1, 1, incommingAngle);
		float etai = 1, etat = material->refraction;
		vec3 n = N;
		if (cosi < 0) { cosi = -cosi; }
		else { swap(etai, etat); n = -N; }
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);
	
		if (k < 0)
		{
			return color;
		}
		else
		{
			Ray* refractionRay = new Ray();
			refractionRay->origin = hitPoint;

			refractionRay->direction = eta * ray->direction + (eta * cosi - sqrtf(k)) * n;
			vec3 hitEpsilon = refractionRay->origin + refractionRay->direction * 0.01;
			refractionRay->origin = hitEpsilon;
			color += this->trace(refractionRay, depth);
			delete refractionRay;
		}

		Ray* reflectionRay = this->computeReflectionRay(ray);
		vec4 reflectionColor = this->trace(reflectionRay, depth);
		delete reflectionRay;

		return material->reflection * reflectionColor + 0.8 * color;
	}

	return BGCOLOR;
}

vec4 Scene::illuminate(Ray* ray)
{
	Ray* reflectionRay = new Ray();
	reflectionRay->origin = ray->origin + ray->t * ray->direction;
	reflectionRay->direction = normalize(this->lightSources[0]->position - reflectionRay->origin);

	vec3 normal = this->primitives[ray->intersectedObjectId]->getNormal(reflectionRay->origin);

	float dotDirectionNormal = dot(reflectionRay->direction, normal);
	if (dotDirectionNormal < 0)
	{
		delete reflectionRay;
		return vec4(0, 0, 0, 0);
	}

	vec3 hitEpsilon = reflectionRay->origin + reflectionRay->direction * 0.01;
	reflectionRay->origin = hitEpsilon;
	this->intersectPrimitives(reflectionRay);

	if (reflectionRay->intersectedObjectId != -1)
	{
		delete reflectionRay;
		return vec4(0, 0, 0, 0);
	}

	float attenuation = reflectionRay->direction.sqrLentgh();
	vec4 color = dotDirectionNormal * attenuation * this->lightSources[0]->color;
	
	delete reflectionRay;
	return color;
}

Ray* Scene::computeReflectionRay(Ray* ray)
{
	vec3 hitPoint = ray->origin + ray->t * ray->direction;
	vec3 N = this->primitives[ray->intersectedObjectId]->getNormal(hitPoint);

	Ray* reflectionRay = new Ray();
	reflectionRay->origin = hitPoint;
	reflectionRay->direction = ray->direction - 2 * (ray->direction * N) * N;

	return reflectionRay;
}

void Scene::intersectPrimitives(Ray* ray)
{
	for (int i = 0; i < this->primitives.size(); i++)
	{
		this->primitives[i]->intersect(ray);
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

void Scene::loadObjModel(const char *filename, Material* material)
{
	// obj file content
	std::vector<vec3> vertices;
	//std::vector<vec2> textures;
	std::vector<int> faceIndexes;
	//std::vector<int> textureIndexes;
	std::vector<vec3> meshVertices;
	//std::vector<vec2> textureCoordinates;

	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		printf("Cannot load %s file!", filename);
	}

	std::string line;
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.substr(0, 2) == "v ") {
			std::istringstream v(line.substr(2));
			vec3 vert;
			float x, y, z;
			v >> x; v >> y; v >> z;
			vert = vec3(x, y, z);
			vertices.push_back(vert);
		}
		//check for texture co-ordinate
		else if (line.substr(0, 2) == "vt") {
			/*std::istringstream v(line.substr(3));
			vec2 tex;
			int U, V;
			v >> U; v >> V;
			tex = vec2(U, V);
			textures.push_back(tex);*/
		}
		//check for faces
		else if (line.substr(0, 2) == "f ") {
			int a, b, c; //to store mesh index
			//int A, B, C; //to store texture index
			//std::istringstream v;
			//v.str(line.substr(2));
			const char* chh = line.c_str();
			sscanf(chh, "f %i %i %i", &a, &b, &c); //here it read the line start with f and store the corresponding values in the variables

			a--; b--; c--;
			//A--; B--; C--;

			faceIndexes.push_back(a); //textureIndexes.push_back(A);
			faceIndexes.push_back(b); //textureIndexes.push_back(B);
			faceIndexes.push_back(c); //textureIndexes.push_back(C);
		}

	}
	//the mesh data is finally calculated here
	for (unsigned int i = 0; i < faceIndexes.size(); i++)
	{
		meshVertices.push_back(
			vec3(vertices[faceIndexes[i]].x, vertices[faceIndexes[i]].y, vertices[faceIndexes[i]].z)
		);
		//vec2 texData = vec2(textures[textureIndexes[i]].x, textures[textureIndexes[i]].y);
		//texCoord.push_back(texData);
	}

	// add triangles to the scene
	int primitivesCount = meshVertices.size() / 3;
	printf("primitives count in %s file: %i\n", filename, primitivesCount);
	for (int i = 0; i < primitivesCount; i++)
	{
		vec3 a = meshVertices[i * 3];
		vec3 b = meshVertices[i * 3 + 1];
		vec3 c = meshVertices[i * 3 + 2];

		Triangle* triangle = new Triangle(material, this->primitives.size(), a, b, c);
		this->primitives.push_back(triangle);
	}
}
