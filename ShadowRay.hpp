#ifndef SHADOWRAY_HPP
#define SHADOWRAY_HPP

#include "vecmath.h"
#include "Ray.hpp"

struct ShadowRay
{
	PackedRay* ray;

	/// color is the light contribution.
	/// will be added to destination if the ray doesn't hit any geometry
	qvec* color;

	quad<vec*>* destination;
};

#endif