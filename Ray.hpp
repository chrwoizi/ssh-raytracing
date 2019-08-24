#ifndef RAY_HPP
#define RAY_HPP

#include "vecmath.h"

#undef RAYSINGLEORIGIN

class Triangle;	// forward declaration

class PackedRay : public SIMDmemAligned
{
  public:
#ifdef RAYSINGLEORIGIN
	vec origin;
#else
	qvec origin;
#endif
	qvec dir;
	qvec dirrcp;
	qfloat t;
	qfloat u,v;
	quad<Triangle*> hit;
};

class SingleRay
{
  public:
	vec origin;
	vec dir;
	vec dirrcp;
	float t;
	float u,v;
	Triangle* hit;
};

#endif
