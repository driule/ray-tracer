#include "precomp.h"

Material::Material(vec4 color, MaterialType type)
{
	this->color = color;
	this->type = type;
}

Primitive::Primitive(Material* material)
{
	this->material = material;
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, vec3 position, float radius) : Primitive(material)
{
	this->position = position;
	this->radius = radius;
	this->radius2 = radius * radius;
	
	this->boundingBox = new BoundingBox(this->position - this->radius, this->position + this->radius);
}

void Sphere::intersect(Ray* ray)
{
	vec3 c = this->position - ray->origin;
	float t = dot(c, ray->direction);
	if (t < 0) return;
	vec3 q = c - t * ray->direction;
	float p2 = dot(q, q);

	if (p2 > this->radius2) return;

	t -= sqrt(this->radius2 - p2);
	if (t < ray->t && t >= EPSILON)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}

vec3 Sphere::getNormal(vec3 point)
{
	return normalize(point - this->position);
}

void Sphere::translate(vec3 vector)
{
	this->position += vector;
	this->boundingBox->translate(vector);
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, vec3 a, vec3 b, vec3 c) : Primitive(material)
{
	this->a = a;
	this->b = b;
	this->c = c;

	float minX = MIN(MIN(this->a.x, this->b.x), this->c.x);
	float minY = MIN(MIN(this->a.y, this->b.y), this->c.y);
	float minZ = MIN(MIN(this->a.z, this->b.z), this->c.z);

	float maxX = MAX(MAX(this->a.x, this->b.x), this->c.x);
	float maxY = MAX(MAX(this->a.y, this->b.y), this->c.y);
	float maxZ = MAX(MAX(this->a.z, this->b.z), this->c.z);

	this->boundingBox = new BoundingBox(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));
}

void Triangle::intersect(Ray* ray)
{
	float t, u, v;

	vec3 ab = this->b - this->a;
	vec3 ac = this->c - this->a;
	vec3 pvec = ray->direction.cross(ac);
	float det = ab.dot(pvec);

	float invDet = 1 / det;

	vec3 tvec = ray->origin - a;
	u = tvec.dot(pvec) * invDet;

	if (u < 0 || u > 1) return;

	vec3 qvec = tvec.cross(ab);
	v = ray->direction.dot(qvec) * invDet;
	if (v < 0 || u + v > 1) return;

	t = ac.dot(qvec) * invDet;
	if (t < ray->t && t >= EPSILON)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}

vec3 Triangle::getNormal(vec3 point)
{
	return normalize(
		cross(this->a - this->b, this->b - this->c)
	);
}

void Triangle::translate(vec3 vector)
{
	this->a += vector;
	this->b += vector;
	this->c += vector;

	this->boundingBox->translate(vector);
}

// -------------------- PLANE ------------------------------------

Plane::Plane(Material* material, vec3 position, vec3 direction) : Primitive(material)
{
	this->position = position;
	this->direction = direction;
}

void Plane::intersect(Ray* ray)
{
	float denominator = dot(this->direction, ray->direction);
	if (abs(denominator) > EPSILON) {
		float t = dot(this->position - ray->origin, this->direction) / denominator;
		if (t < ray->t && t >= EPSILON)
		{
			ray->t = t;
			ray->intersectedObjectId = this->id;
		}
	}
}

vec3 Plane::getNormal(vec3 point)
{
	return normalize(this->direction);
}

void Plane::translate(vec3 vector)
{
	this->position += vector;
}

// -------------------- CYLINDER ------------------------------------

Cylinder::Cylinder(Material* material, vec3 position, vec3 upVector, float radius, float height) : Primitive(material)
{
	this->position = position;
	this->upVector = upVector;
	this->radius = radius;
	this->height = height;

	float boundingRadius = MAX(this->radius, this->height);

	vec3 size;
	size.x = upVector.x * height + 2 * radius * sqrt(1 - upVector.x * upVector.x);
	size.y = upVector.y * height + 2 * radius * sqrt(1 - upVector.y * upVector.y);
	size.z = upVector.z * height + 2 * radius * sqrt(1 - upVector.z * upVector.z);

	this->boundingBox = new BoundingBox(this->position - size, this->position + size);
}

void Cylinder::intersect(Ray* ray)
{
	vector<float> points;
	vec3 alpha = upVector * ray->direction.dot(upVector);
	vec3 deltaPosition = (ray->origin - this->position);
	vec3 beta = upVector * deltaPosition.dot(upVector);
	vec3 center2 = this->position + upVector * height;

	float a = (ray->direction - alpha).sqrLentgh();
	float b = 2 * ((ray->direction - alpha).dot(deltaPosition - beta));
	float c = (deltaPosition - beta).sqrLentgh() - radius*radius;

	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) { return; }
	else
	{
		discriminant = sqrt(discriminant);
		float t1 = ((-1 * b) + discriminant) / (2 * a);
		float t2 = ((-1 * b) - discriminant) / (2 * a);
		if (t1 >= 0)
		{
			if (upVector.dot((ray->origin - this->position) + ray->direction * t1) > 0 && upVector.dot((ray->origin - center2) + ray->direction * t1) < 0)
			{
				points.push_back(t1);
			}
		}
		if (t2 >= 0)
		{
			if (upVector.dot((ray->origin - this->position) + ray->direction * t2) > 0 && upVector.dot((ray->origin - center2) + ray->direction * t2) < 0)
			{
				points.push_back(t2);
			}
		}
	}

	float denominator = ray->direction.dot(upVector);
	if (denominator > EPSILON)
	{
		vec3 co = this->position - ray->origin;
		float t3 = co.dot(upVector) / denominator;
		if (t3 > 0 && (ray->direction * t3 - co).sqrLentgh() <= radius*radius)
		{
			points.push_back(t3);
		}
	}
	else if (denominator < EPSILON)
	{
		vec3 co2 = center2 - ray->origin;
		float t4 = co2.dot(upVector) / denominator;
		if (t4 > 0 && (ray->direction * t4 - co2).sqrLentgh() <= radius*radius)
		{
			points.push_back(t4);
		}
	}

	float minT = INFINITY;
	bool intersects = false;
	for (int i = 0; i<points.size(); i++)
	{
		if (minT > points[i] && points[i] >= EPSILON)
		{
			minT = points[i];
			intersects = true;
		}
	}

	if (intersects && minT < ray->t)
	{
		ray->t = minT / ray->direction.length();
		ray->intersectedObjectId = this->id;
	}
}

vec3 Cylinder::getNormal(vec3 point)
{
	vec3 co = point - this->position;
	vec3 co2 = co - upVector * height;

	if (co.sqrLentgh() <= radius * radius)
	{
		return this->upVector;
	}
	if (co2.sqrLentgh() <= radius * radius)
	{
		return this->upVector;
	}

	return normalize(co - co.project(upVector));
}

void Cylinder::translate(vec3 vector)
{
	this->position += vector;
	this->boundingBox->translate(vector);
}

// -------------------- TORUS ------------------------------------

Torus::Torus(Material* material, float R, float r, vec3 position, vec3 axis) : Primitive(material)
{
	this->position = position;
	this->R = R;
	this->r = r;
	this->axis = normalize(axis);

	this->boundingBox = new BoundingBox(this->position - (this->R + this->r), this->position + (this->R + this->r));
}

void Torus::intersect(Ray* ray)
{
	vec3 centerToRayOrigin = ray->origin - position;
	long double centerToRayOriginDotDirectionSquared = dot(centerToRayOrigin, centerToRayOrigin);
	long double r2 = r * r;
	long double R2 = R * R;

	long double axisDotCenterToRayOrigin = dot(axis, centerToRayOrigin);
	long double axisDotRayDirection = dot(axis, ray->direction);
	long double a = 1 - axisDotRayDirection * axisDotRayDirection;
	long double b = 2 * (dot(centerToRayOrigin, ray->direction) - axisDotCenterToRayOrigin * axisDotRayDirection);
	long double c = centerToRayOriginDotDirectionSquared - axisDotCenterToRayOrigin * axisDotCenterToRayOrigin;
	long double d = centerToRayOriginDotDirectionSquared + R2 - r2;

	// Solve quartic equation with coefficients A, B, C, D and E
	long double A = 1;
	long double B = 4 * dot(ray->direction, centerToRayOrigin);
	long double C = 2 * d + B * B * 0.25f - 4 * R2 * a;
	long double D = B * d - 4 * R2 * b;
	long double E = d * d - 4 * R2 * c;

	// Maximum number of roots is 4
	QuarticEquation equation(A, B, C, D, E);
	const int maxRootsCount = 4;
	double roots[maxRootsCount] = { -1.0, -1.0, -1.0, -1.0 };
	int rootsCount = equation.Solve(roots);

	if (rootsCount == 0) { return; }

	// Find closest to zero positive solution
	float closestRoot = INFINITY;
	for (int i = 0; i < maxRootsCount; ++i)
	{
		if (roots[i] >= EPSILON && roots[i] < closestRoot)
		{
			closestRoot = roots[i];
		}
	}

	if (closestRoot < ray->t)
	{
		ray->t = closestRoot;
		ray->intersectedObjectId = this->id;
	}
}

vec3 Torus::getNormal(vec3 point)
{
	vec3 P = (point - this->position);
	float A = (this->R - this->r) / 2 + this->r;
	float alpha = A / (sqrtf(P.x * P.x + P.y * P.y));
	vec3 Q = vec3(alpha * P.x, alpha * P.y, 0);
	vec3 N = normalize(P - Q);

	return N;
}

void Torus::translate(vec3 vector)
{
	this->position += vector;
	this->boundingBox->translate(vector);
}
