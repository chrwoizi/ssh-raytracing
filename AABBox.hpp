#ifndef AABBOX_HPP
#define AABBOX_HPP

#include <cmath>

#include "Ray.hpp"
#include "simd/simd.h"

#include "bigfloat.h"

#include "TestConfig.hpp"

#ifndef INFINITY
#define INFINITY 1e8
#endif

/* axis aligned bounding box */
class AABBox
{
  public:
	vec min,max;
	
	AABBox()
	{ clear(); }
	
	inline bool operator==(const AABBox &rh) const
	{
		return (min.x == rh.min.x) && (min.y == rh.min.y) && (min.z == rh.min.z)
			&& (max.x == rh.max.x) && (max.y == rh.max.y) && (max.z == rh.max.z);
	}
	
	inline void clear()
	{
		min =  INFINITY;
		max = -INFINITY;	
	}
	
	inline void extend(const vec &v)
	{
		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.z < min.z) min.z = v.z;
		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
		if (v.z > max.z) max.z = v.z;
	}
	
	inline void extend(const AABBox &box)
	{
		if (box.min.x < min.x) min.x = box.min.x;
		if (box.min.y < min.y) min.y = box.min.y;
		if (box.min.z < min.z) min.z = box.min.z;
		if (box.max.x > max.x) max.x = box.max.x;
		if (box.max.y > max.y) max.y = box.max.y;
		if (box.max.z > max.z) max.z = box.max.z;
	}
	
	inline void join(const AABBox &box)
	{
		if (box.min.x > min.x) min.x = box.min.x;
		if (box.min.y > min.y) min.y = box.min.y;
		if (box.min.z > min.z) min.z = box.min.z;
		if (box.max.x < max.x) max.x = box.max.x;
		if (box.max.y < max.y) max.y = box.max.y;
		if (box.max.z < max.z) max.z = box.max.z;
		if (min.x > max.x || min.y > max.y || min.x > max.x)
			clear();
	}
	
	inline bool overlaps(const AABBox &b) const
	{
		if (max.x < b.min.x || b.max.x < min.x)
			return false;
		if (max.y < b.min.y || b.max.y < min.y)
			return false;
		if (max.z < b.min.z || b.max.z < min.z)
			return false;
		return true;
	}
	
	inline bool contains(const vec &point) const
	{
		if (point.x < min.x || max.x < point.x)
			return false;
		if (point.y < min.y || max.y < point.y)
			return false;
		if (point.z < min.z || max.z < point.z)
			return false;
		return true;
	}
	
	inline bool contains(const AABBox &b) const
	{
		if (b.min.x < min.x || max.x < b.max.x)
			return false;
		if (b.min.y < min.y || max.y < b.max.y)
			return false;
		if (b.min.z < min.z || max.z < b.max.z)
			return false;
		return true;
	}
	
	void clip(const PackedRay &ray, qfloat &tnear, qfloat &tfar) const
	{
		qfloat tynear, tyfar, tznear, tzfar;
		
		qmask m = ray.dirrcp.x >= 0.0f;
		qfloat minval = (min.x - ray.origin.x) * ray.dirrcp.x;
		qfloat maxval = (max.x - ray.origin.x) * ray.dirrcp.x;
		tnear.condAssign(m, minval, maxval);
		tfar.condAssign(m, maxval, minval);

		m = ray.dirrcp.y >= 0.0f;
		minval = (min.y - ray.origin.y) * ray.dirrcp.y;
		maxval = (max.y - ray.origin.y) * ray.dirrcp.y;
		tynear.condAssign(m, minval, maxval);
		tyfar.condAssign(m, maxval, minval);
		
		tnear.condAssign(tynear > tnear, tynear, tnear);
		tfar.condAssign(tyfar < tfar, tyfar, tfar);
		
		m = ray.dirrcp.z >= 0.0f;
		minval = (min.z - ray.origin.z) * ray.dirrcp.z;
		maxval = (max.z - ray.origin.z) * ray.dirrcp.z;
		tznear.condAssign(m, minval, maxval);
		tzfar.condAssign(m, maxval, minval);
		
		tnear.condAssign(tznear > tnear, tznear, tnear);
		tfar.condAssign(tzfar < tfar, tzfar, tfar);
	}

#ifdef BIGFLOAT_SURFACE_COMPUTATION
	BigFloat surfaceAreaPrecise() const
	{
		vec extend = max - min;
		return BigFloat(2.0)*
				((BigFloat(extend.x)*BigFloat(extend.y)) +
				(BigFloat(extend.y)*BigFloat(extend.z)) +
				(BigFloat(extend.z)*BigFloat(extend.x)));
	}
#else
	long double surfaceAreaPrecise() const
	{
		vec extend = max - min;
		return (long double)(2.0)*
				(((long double)(extend.x)*(long double)(extend.y)) +
				((long double)(extend.y)*(long double)(extend.z)) +
				((long double)(extend.z)*(long double)(extend.x)));
	}
#endif
	
	float surfaceArea() const
	{
		vec extend = max - min;
		return 2.0*(extend.x*extend.y + extend.y*extend.z + extend.z*extend.x);
	}
};

#endif
