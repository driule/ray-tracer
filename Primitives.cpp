#include "precomp.h"

Material::Material(vec4 color, MaterialType type)
{
	this->color = color;
	this->type = type;
}

Primitive::Primitive(Material* material, int id)
{
	this->material = material;
	this->id = id;
}

// -------------------- SPHERE ------------------------------------

Sphere::Sphere(Material* material, int id, vec3 position, float radius) : Primitive(material, id)
{
	this->position = position;
	this->radius = radius;
	this->radius2 = radius * radius;
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
	if ((t < ray->t) && t > 0)
	{
		ray->t = t;
		ray->intersectedObjectId = this->id;
	}
}

vec3 Sphere::getNormal(vec3 point)
{
	return normalize(point - this->position);
}

// -------------------- TRIANGLE ------------------------------------

Triangle::Triangle(Material* material, int id, vec3 a, vec3 b, vec3 c) : Primitive(material, id)
{
	this->a = a;
	this->b = b;
	this->c = c;
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
	if (t < ray->t && t > 0)
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

// -------------------- PLANE ------------------------------------

Plane::Plane(Material* material, int id, vec3 position, vec3 direction) : Primitive(material, id)
{
	this->position = position;
	this->direction = direction;
}

void Plane::intersect(Ray* ray)
{
	float denominator = dot(this->direction, ray->direction);
	if (abs(denominator) > 1e-6) {
		float t = dot(this->position - ray->origin, this->direction) / denominator;
		if (t < ray->t && t >= 1e-6)
		{
			ray->t = t;
			ray->intersectedObjectId = this->id;
		}
	}
}

vec3 Plane::getNormal(vec3 point)
{
	return this->direction;
}

// -------------------- CYLINDER ------------------------------------

Cylinder::Cylinder(Material* material, int id, vec3 position, vec3 upVector, float radius, float height) : Primitive(material, id)
{
	this->position = position;
	this->upVector = upVector;
	this->radius = radius;
	this->height = height;
}

void Cylinder::intersect(Ray* ray)
{
	double Ird = ray->direction.length();

	vector<double> points;
	vec3 alpha = upVector * ray->direction.dot(upVector);
	vec3 deltaP = (ray->origin - this->position);
	vec3 beta = upVector * deltaP.dot(upVector);
	vec3 center2 = this->position + upVector*height;

	double a = (ray->direction - alpha).sqrLentgh();
	double b = 2 * ((ray->direction - alpha).dot(deltaP - beta));
	double c = (deltaP - beta).sqrLentgh() - radius*radius;

	double discriminant = b*b - 4 * a*c;
	if (discriminant < 0) return;
	else {
		discriminant = sqrt(discriminant);
		double t1 = ((-1 * b) + discriminant) / (2 * a);
		double t2 = ((-1 * b) - discriminant) / (2 * a);
		if (t1 >= 0) {
			if (upVector.dot((ray->origin - this->position) + ray->direction * t1)>0 && upVector.dot((ray->origin - center2) + ray->direction * t1)<0)
				points.push_back(t1);
		}
		if (t2 >= 0)
			if (upVector.dot((ray->origin - this->position) + ray->direction * t2)>0 && upVector.dot((ray->origin - center2) + ray->direction * t2)<0)
				points.push_back(t2);
	}

	float denom = ray->direction.dot(upVector);
	if (denom > 1e-6) {
		vec3 co = this->position - ray->origin;
		double t3 = co.dot(upVector) / denom;
		if (t3 > 0 && (ray->direction * t3 - co).sqrLentgh() <= radius*radius)
			points.push_back(t3);
	}
	else if (denom < 1e-6) {
		vec3 co2 = center2 - ray->origin;
		double t4 = co2.dot(upVector) / denom;
		if (t4 > 0 && (ray->direction * t4 - co2).sqrLentgh() <= radius*radius)
			points.push_back(t4);
	}

	double minT = INFINITY;
	bool flag = false;
	for (int i = 0; i<points.size(); i++) {
		if (minT > points[i] && points[i] >= 0) {
			minT = points[i];
			flag = true;
		}
	}
	if (flag) {
		ray->t = minT / Ird;
		ray->intersectedObjectId = this->id;
	}
}

vec3 Cylinder::getNormal(vec3 point)
{
	vec3 co = point - this->position;
	vec3 co2 = co - upVector * height;

	if (co.sqrLentgh() <= radius * radius)
		return this->upVector;
	if (co2.sqrLentgh() <= radius * radius)
		return this->upVector;

	return normalize(co - co.project(upVector));
}