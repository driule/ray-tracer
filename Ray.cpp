#include "precomp.h"

Ray::Ray()
{
	this->t = INFINITY;
	this->intersectedObjectId = -1;
	this->isInAir = false;
}
