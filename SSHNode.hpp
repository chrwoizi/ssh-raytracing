#ifndef SSHNODE_HPP
#define SSHNODE_HPP

#include "XHierarchyConfig.hpp"

struct SSHNode
{
	// flags //

	enum FLAGS {
		NEAR_FLAG = 8,	// b001000	the geometry is on the lower side of the slab (slab at x=5 --> geometry bounds x < 5)
		LEAF_FLAG = 4	// b000100	this is a leaf node
	};
	enum AXIS
	{
		AXIS_X = 0,	// b00000	the slab is orthogonal to the x axis
		AXIS_Y = 1,	// b00001	the slab is orthogonal to the y axis
		AXIS_Z = 2	// b00010	the slab is orthogonal to the z axis
	};

	#ifdef TRAVERSE_ORDERED
		// 6 bits for flags: splitaxis, near, leaf/slabaxis
		#define SSH_FLAG_MASK_BITS 6
		#define SSH_FLAG_MASK 63
	#else
		// 4 bits for flags: near, leaf/slabaxis
		#define SSH_FLAG_MASK_BITS 4
		#define SSH_FLAG_MASK 15
	#endif

	#define SSH_GEOM_OR_CHILD_INDEX_MASK_BITS 	(sizeof(x_node_child_id_t)-SSH_FLAG_MASK_BITS)
	#define SSH_GEOM_OR_CHILD_INDEX_MASK 		(~SSH_FLAG_MASK)

	#define SSH_PACK_GEOM_OR_CHILD_INDEX(v) (((v)<<SSH_FLAG_MASK_BITS)&SSH_GEOM_OR_CHILD_INDEX_MASK)
	#define SSH_UNPACK_GEOM_OR_CHILD_INDEX(v) (((v) & SSH_GEOM_OR_CHILD_INDEX_MASK)>>SSH_FLAG_MASK_BITS)

	// data //

	union {
		x_node_child_id_t geo_child_index;
		x_node_child_id_t flags;
	};
	float plane;

	static const unsigned long memSize = sizeof(x_node_child_id_t) + sizeof(float);
	
	// methods //

	#ifdef TRAVERSE_ORDERED
		inline x_node_child_id_t getSplitAxis() const { return (flags >> 4) & 0x03; }
		inline void setSplitAxis(AXIS axis) { flags |= (x_node_child_id_t)axis << 4; }
	#endif

	inline void setSlab(AXIS _axis, bool _near, float pos) { flags = ((_near ? (_axis|NEAR_FLAG) : _axis) & SSH_FLAG_MASK); plane = pos; }
	inline x_node_child_id_t getSlabAxis() const { return flags & 0x03; }

	inline void setLeaf(x_node_child_id_t geomIndex) { geo_child_index = SSH_PACK_GEOM_OR_CHILD_INDEX(geomIndex) | (flags&SSH_FLAG_MASK) | (x_node_child_id_t)LEAF_FLAG; }
	inline x_node_child_id_t getGeomIndex() const { return SSH_UNPACK_GEOM_OR_CHILD_INDEX(geo_child_index); }

	inline void setInner(x_node_child_id_t childId) { geo_child_index = SSH_PACK_GEOM_OR_CHILD_INDEX(childId) | (flags&SSH_FLAG_MASK); }
	inline x_node_child_id_t getChildId() const { return SSH_UNPACK_GEOM_OR_CHILD_INDEX(geo_child_index); }

	inline bool isLeaf() const { return flags & (x_node_child_id_t)LEAF_FLAG; }
	inline bool isNear() const { return flags & (x_node_child_id_t)NEAR_FLAG; }
};

#endif

