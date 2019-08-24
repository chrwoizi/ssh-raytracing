#ifndef BVHNODE_HPP
#define BVHNODE_HPP

#include "XHierarchyConfig.hpp"

#include "vecmath.h"

struct BVHNode
{
	// flags //

	// flags are in the least significant bits
	enum FLAGS {
		#ifdef TRAVERSE_ORDERED
			// 2 bits for flags. leaf and split axis
			#define BVH_FLAG_MASK_BITS 2
			#define BVH_FLAG_MASK 3
			LEAF_FLAG = 3,	// b11	this is a leaf node
			AXIS_X = 0,		// b00	the nodes are split orthogonal to the x axis
			AXIS_Y = 1,		// b01	the nodes are split orthogonal to the y axis
			AXIS_Z = 2		// b10	the nodes are split orthogonal to the z axis
		#else
			// 1 bit for flags. leaf or not
			#define BVH_FLAG_MASK_BITS 1
			#define BVH_FLAG_MASK 1
			LEAF_FLAG = 1	// b1	this is a leaf node
		#endif
	};

	#define BVH_GEOM_OR_CHILD_INDEX_MASK_BITS 	(sizeof(x_node_child_id_t)-BVH_FLAG_MASK_BITS)
	#define BVH_GEOM_OR_CHILD_INDEX_MASK 		(~BVH_FLAG_MASK)

	#define BVH_PACK_GEOM_OR_CHILD_INDEX(v) (((v)<<BVH_FLAG_MASK_BITS)&BVH_GEOM_OR_CHILD_INDEX_MASK)
	#define BVH_UNPACK_GEOM_OR_CHILD_INDEX(v) (((v) & BVH_GEOM_OR_CHILD_INDEX_MASK)>>BVH_FLAG_MASK_BITS)

	// data //

	union {
		x_node_child_id_t geo_child_index;
		x_node_child_id_t flags;
	};
	vec min;
	vec max;
	
	static const unsigned long memSize = sizeof(x_node_child_id_t) + 2*sizeof(vec);
	
	// methods //

	#ifdef TRAVERSE_ORDERED
		inline x_node_child_id_t getSplitAxis() const { return flags & BVH_FLAG_MASK; }
		inline void setSplitAxis(FLAGS axis) { flags |= (x_node_child_id_t)axis; }
	#endif

	inline void setLeaf(x_node_child_id_t geomIndex) { geo_child_index = BVH_PACK_GEOM_OR_CHILD_INDEX(geomIndex) | (x_node_child_id_t)LEAF_FLAG; }
	inline x_node_child_id_t getGeomIndex() const { return BVH_UNPACK_GEOM_OR_CHILD_INDEX(geo_child_index); }

	inline void setInner(x_node_child_id_t childId) { geo_child_index = BVH_PACK_GEOM_OR_CHILD_INDEX(childId); }
	inline x_node_child_id_t getChildId() const { return BVH_UNPACK_GEOM_OR_CHILD_INDEX(geo_child_index); }

	inline bool isLeaf() const { return (flags & BVH_FLAG_MASK) == (x_node_child_id_t)LEAF_FLAG; }
};

#endif