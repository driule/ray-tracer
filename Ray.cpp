#include "precomp.h"

Ray::Ray()
{
	this->t = INFINITY;
	this->intersectedPrimitiveId = -1;
}
