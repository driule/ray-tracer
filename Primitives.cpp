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

void Primitive::intersectBoundingBox(Ray* ray)
{
	float tmin = (this->boundingBoxMin.x - ray->origin.x) / ray->direction.x;
	float tmax = (this->boundingBoxMax.x - ray->origin.x) / ray->direction.x;

	if (tmin > tmax) swap(tmin, tmax);

	float tymin = (this->boundingBoxMin.y - ray->origin.y) / ray->direction.y;
	float tymax = (this->boundingBoxMax.y - ray->origin.y) / ray->direction.y;

	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return; // No intersection

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (this->boundingBoxMin.z - ray->origin.z) / ray->direction.z;
	float tzmax = (this->boundingBoxMax.z - ray->origin.z) / ray->direction.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return; // no intersection

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	if (tmin < ray->t)
	{
		ray->t = tmin;
		ray->intersectedObjectId = this->id;
	}
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, vec3 position, float radius) : Primitive(material)
{
	this->position = position;
	this->radius = radius;
	this->radius2 = radius * radius;
	this->boundingBoxMin = position - radius;
	this->boundingBoxMax = position + radius;
	this->center = position;
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
	this->center = this->position;
	this->boundingBoxMin = position - radius;
	this->boundingBoxMax = position + radius;
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, vec3 a, vec3 b, vec3 c) : Primitive(material)
{
	this->a = a;
	this->b = b;
	this->c = c;

	float minX = MIN(MIN(a.x, b.x), c.x);
	float minY = MIN(MIN(a.y, b.y), c.y);
	float minZ = MIN(MIN(a.z, b.z), c.z);

	float maxX = MAX(MAX(a.x, b.x), c.x);
	float maxY = MAX(MAX(a.y, b.y), c.y);
	float maxZ = MAX(MAX(a.z, b.z), c.z);

	this->boundingBoxMin = vec3(minX, minY, minZ);
	this->boundingBoxMax = vec3(maxX, maxY, maxZ);

	this->center = (a + b + c) / 3;
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

void Triangle::translate(vec3 vector)
{
	this->a += vector;
	this->b += vector;
	this->b += vector;

	float minX = MIN(MIN(a.x, b.x), c.x);
	float minY = MIN(MIN(a.y, b.y), c.y);
	float minZ = MIN(MIN(a.z, b.z), c.z);

	float maxX = MAX(MAX(a.x, b.x), c.x);
	float maxY = MAX(MAX(a.y, b.y), c.y);
	float maxZ = MAX(MAX(a.z, b.z), c.z);

	this->boundingBoxMin = vec3(minX, minY, minZ);
	this->boundingBoxMax = vec3(maxX, maxY, maxZ);

	this->center = (a + b + c) / 3;
}

vec3 Triangle::getNormal(vec3 point)
{
	return normalize(
		cross(this->a - this->b, this->b - this->c)
	);
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

	this->boundingBoxMin = position - radius;
	this->boundingBoxMax = position + radius;

	this->center = position + height * upVector * 0.5;
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

	this->boundingBoxMin = position - radius;
	this->boundingBoxMax = position + radius;

	this->center = position + height * upVector * 0.5;
}

// -------------------- TORUS ------------------------------------

Torus::Torus(Material* material, float R, float r, vec3 position, vec3 axis) : Primitive(material)
{
	this->position = position;
	this->R = R;
	this->r = r;
	this->axis = normalize(axis);

	this->boundingBoxMin = position - (R + r);
	this->boundingBoxMax = position + (R + r);

	this->center = position;
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

	//vec3 centerToPoint = point - position;
	//float centerToPointDotAxis = dot(axis, centerToPoint);
	//vec3 direction = normalize(centerToPoint - axis * centerToPointDotAxis);

	//return normalize(point - position + direction * R);
}

void Torus::translate(vec3 vector)
{
	this->position += vector;

	this->boundingBoxMin = this->position - (R + r);
	this->boundingBoxMax = this->position + (R + r);

	this->center = position;
}