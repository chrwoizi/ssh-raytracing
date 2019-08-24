#include <cassert>
#include <iostream>

#include "XHierarchy.hpp"
#include "Triangle.hpp"

void SingleSlabHierarchy::updateActiveRaySegment(const PackedRay& ray, const qmask reverse[3], const SSHNode* node, qfloat& t_near, qfloat& t_far)
{
	unsigned long axis = node->getSlabAxis();

	qfloat t = (qfloat(node->plane) - ray.origin[axis]) * ray.dirrcp[axis];

	// update active ray segment

	#if 0
	// funktioniert, falls alle Strahlen die gleichen Richtungsvorzeichen haben
	if(node->isNear()){
		if(reverse[axis].mask()){
			t_far.condAssign(t<t_far, t, t_far);
		}
		else{
			t_near.condAssign(t>t_near, t, t_near);
		}
	}
	else{
		if(reverse[axis].mask()){
			t_near.condAssign(t>t_near, t, t_near);
		}
		else{
			t_far.condAssign(t<t_far, t, t_far);
		}
	}
	#endif

	#if 0
	// funktioniert, falls alle Strahlen die gleichen Richtungsvorzeichen haben
	if(reverse[axis].mask()){
		if(node->isNear()){
			t_far.condAssign(t<t_far, t, t_far);
		}
		else{
			t_near.condAssign(t>t_near, t, t_near);
		}
	}
	else{
		if(node->isNear()){
			t_near.condAssign(t>t_near, t, t_near);
		}
		else{
			t_far.condAssign(t<t_far, t, t_far);
		}
	}
	#endif

	#if 0
	// funktioniert, falls alle Strahlen die gleichen Richtungsvorzeichen haben
	if(reverse[axis].mask() ^ (SSH_UNPACK_FLAGS(node->flags) & SSHNode::NEAR_FLAG)){
		// update t_near if necessary, keep t_far
		t_near.condAssign(t > t_near, t, t_near);
	} else {
		// keep t_near, update t_far if necessary
		t_far.condAssign(t < t_far, t, t_far);
	}
	#endif

	// funktioniert auch mit Strahlen, die unterschiedliche Richtungsvorzeichen haben
	if (node->isNear())
	{
		// near plane
		t_near.condAssign((reverse[axis] | (t<=t_near)), t_near, t);	// increase t_near
		t_far.condAssign((reverse[axis] & (t<t_far)), t, t_far);	// decrease t_far
	}
	else
	{
		// far plane
		t_near.condAssign((reverse[axis] & (t>t_near)), t, t_near);	// increase t_near
		t_far.condAssign((reverse[axis] | (t>=t_far)), t_far, t);	// decrease t_far
	}
}

void BoundingVolumeHierarchy::updateActiveRaySegment(const PackedRay &ray, const qmask reverse[3], const BVHNode *node, qfloat& t_near, qfloat& t_far)
{
	qfloat txnear, txfar, tynear, tyfar, tznear, tzfar;
	
	// x axis slab
	qmask m = ray.dirrcp.x >= 0.0f;
	qfloat minval = (node->min.x - ray.origin.x) * ray.dirrcp.x;
	qfloat maxval = (node->max.x - ray.origin.x) * ray.dirrcp.x;
	txnear.condAssign(m, minval, maxval);
	txfar.condAssign(m, maxval, minval);
	
	t_near.condAssign(txnear > t_near, txnear, t_near);
	t_far.condAssign(txfar < t_far, txfar, t_far);
	
	// y axis slab
	m = ray.dirrcp.y >= 0.0f;
	minval = (node->min.y - ray.origin.y) * ray.dirrcp.y;
	maxval = (node->max.y - ray.origin.y) * ray.dirrcp.y;
	tynear.condAssign(m, minval, maxval);
	tyfar.condAssign(m, maxval, minval);
	
	t_near.condAssign(tynear > t_near, tynear, t_near);
	t_far.condAssign(tyfar < t_far, tyfar, t_far);
	
	// z axis slab
	m = ray.dirrcp.z >= 0.0f;
	minval = (node->min.z - ray.origin.z) * ray.dirrcp.z;
	maxval = (node->max.z - ray.origin.z) * ray.dirrcp.z;
	tznear.condAssign(m, minval, maxval);
	tzfar.condAssign(m, maxval, minval);
	
	t_near.condAssign(tznear > t_near, tznear, t_near);
	t_far.condAssign(tzfar < t_far, tzfar, t_far);
}