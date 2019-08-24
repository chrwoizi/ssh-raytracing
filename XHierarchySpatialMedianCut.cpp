#include <iostream>

#include "XHierarchySpatialMedianCut.hpp"
#include "Triangle.hpp"
#include "TimeMeasurement.hpp"

#include "TestConfig.hpp"

extern bool makeStats;
extern TimeMeasurement constructionTimeMeasurement;

void SingleSlabHierarchySpatialMedianCut::setupRootNode(SSHNode& node, const AABBox &bounds)
{
	node.setSlab(SSHNode::AXIS_X, false, bounds.max.x);
}

/* 
	goal: carve parent bounds by one side
	for each side: carve parent bounds and save the side if resulting volume is the smallest
*/
AABBox SingleSlabHierarchySpatialMedianCut::setNodeVolume(SSHNode& node, const AABBox& parentBounds, const AABBox& bounds, SceneConstructionDetails& out)
{
#if 1
	AABBox candidateBounds(parentBounds);
	candidateBounds.min.x = bounds.min.x;
	AABBox nodeBounds(candidateBounds);
	float a = candidateBounds.surfaceArea();
	node.setSlab(SSHNode::AXIS_X, true, bounds.min.x);
#else
	AABBox nodeBounds;
	AABBox candidateBounds(parentBounds);
	float a = parentBounds.surfaceArea();

	// initialize
	node.setSlab(SSHNode::AXIS_X, true, bounds.min.x);

	// x axis
	candidateBounds.min.x = bounds.min.x;
	nodeBounds = candidateBounds;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		//node.setSlab(SSHNode::AXIS_X, true, bounds.min.x);
		nodeBounds = candidateBounds;
	}
#endif
	candidateBounds = parentBounds;
	candidateBounds.max.x = bounds.max.x;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		node.setSlab(SSHNode::AXIS_X, false, bounds.max.x);
		nodeBounds = candidateBounds;
	}
	// y axis
	candidateBounds = parentBounds;
	candidateBounds.min.y = bounds.min.y;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		node.setSlab(SSHNode::AXIS_Y, true, bounds.min.y);
		nodeBounds = candidateBounds;
	}
	candidateBounds = parentBounds;
	candidateBounds.max.y = bounds.max.y;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		node.setSlab(SSHNode::AXIS_Y, false, bounds.max.y);
		nodeBounds = candidateBounds;
	}
	// z axis
	candidateBounds = parentBounds;
	candidateBounds.min.z = bounds.min.z;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		node.setSlab(SSHNode::AXIS_Z, true, bounds.min.z);
		nodeBounds = candidateBounds;
	}
	candidateBounds = parentBounds;
	candidateBounds.max.z = bounds.max.z;
	if ( candidateBounds.surfaceArea() < a )
	{
		a = candidateBounds.surfaceArea();
		node.setSlab(SSHNode::AXIS_Z, false, bounds.max.z);
		nodeBounds = candidateBounds;
	}

	if(makeStats)
	{
		constructionTimeMeasurement.pause();

		// compute surface ratio for debug/test
#ifdef BIGFLOAT_SURFACE_COMPUTATION
		BigFloat bvh = bounds.surfaceAreaPrecise();
		BigFloat ssh = nodeBounds.surfaceAreaPrecise();

		static long cur = 0;
		static long count = out.innerNodes + out.leafNodes;
		if(count != out.innerNodes + out.leafNodes)
		{
			count = out.innerNodes + out.leafNodes;
			cur = 0;
		}
		++cur;
		if(cur % 1000 == 0) std::cout << cur << "/" << count << std::endl;

		if(bvh.compare(&BigFloat(0.0)) < 0 && ssh.compare(&BigFloat(0.0)) < 0)
		{
			assert(ssh.compare(&bvh) <= 0);
			out.bshSurfaceRatio += (ssh / bvh) / BigFloat(out.innerNodes+out.leafNodes);
		}
		else
		{
			out.bshSurfaceRatio += BigFloat(1.0) / BigFloat(out.innerNodes+out.leafNodes);
		}
#else
		long double bvh = bounds.surfaceAreaPrecise();
		long double ssh = nodeBounds.surfaceAreaPrecise();

		if(bvh > 0 && ssh > 0)
		{
			assert(ssh >= bvh);
			out.bshSurfaceRatio += (ssh / bvh) / (long double)(out.innerNodes+out.leafNodes);
		}
		else
		{
			out.bshSurfaceRatio += (long double)(1.0) / (long double)(out.innerNodes+out.leafNodes);
		}
#endif

		constructionTimeMeasurement.resume();
	}
	
	return nodeBounds;
}

void BoundingVolumeHierarchySpatialMedianCut::setupRootNode(BVHNode& node, const AABBox &bounds)
{
	node.min = bounds.min;
	node.max = bounds.max;
	node.flags = 0;	// not leaf
}

AABBox BoundingVolumeHierarchySpatialMedianCut::setNodeVolume(BVHNode& node, const AABBox& parentBounds, const AABBox& bounds, SceneConstructionDetails& out)
{
	node.min = bounds.min;
	node.max = bounds.max;
	return bounds;
}

