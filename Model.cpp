#include "precomp.h"

Model::Model(std::vector<Primitive*> primitives)
{
	this->primitives = primitives;
}

void Model::translate(vec3 vector)
{
	for (int i = 0; i < this->primitives.size(); i++)
	{
		this->primitives[i]->translate(vector);
	}
}
