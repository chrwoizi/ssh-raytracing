#ifndef REFXXCTIONRAY_HPP
#define REFXXCTIONRAY_HPP

#include "vecmath.h"
#include "Ray.hpp"

struct RefxxctionRay
{
	PackedRay* ray;

	quad<vec*>* destination;

	qvec* contribution;

	int level;
};

#endif