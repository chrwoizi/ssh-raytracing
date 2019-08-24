#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>

#include "AABBox.hpp"
#include "Ray.hpp"
#include "simd/simd.h"

#include "SceneConstructionDetails.hpp"
#include "IntersectDetails.hpp"

#include "Triangle.hpp"

enum SCENE_TYPE
{
	BVH,
	SSH,
	KD,
	SIMPLE
};

class Scene
{
  public:
	virtual ~Scene() {}
	virtual SceneConstructionDetails construct(std::vector<Triangle>* geometries) = 0;
	virtual IntersectDetails intersect(PackedRay&) = 0;
	virtual const AABBox& getBounds() const = 0;
	virtual unsigned long getComputedMemoryUsage() const = 0;
};

#endif
