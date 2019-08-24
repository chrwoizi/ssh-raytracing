#ifndef XHIERARCHYSPATIALMEDIANCUT_HPP
#define XHIERARCHYSPATIALMEDIANCUT_HPP

#include "XHierarchy.hpp"

/*
	common algorithms of BVH and SSH creation.
*/
template<typename Node>
class XHierarchySpatialMedianCut : public XHierarchyConstructionStrategy<Node>
{
public:
	virtual ~XHierarchySpatialMedianCut() {}

	virtual void construct(
		std::vector<Triangle> &globalgeom,
		const AABBox &bounds,
		Node* nodes,
		SceneConstructionDetails& out
	)
	{
		innerNodeCount = 0;
		leafNodeCount = 0;
		
		unsigned long firstFreeElementInNodes = 1;
		
		std::vector<x_node_child_id_t> localgeom(globalgeom.size());
		for(unsigned long i = 0; i < globalgeom.size(); ++i)
		{
			localgeom[i] = i;
		}

		out.leafNodes = globalgeom.size();
		out.innerNodes = globalgeom.size()-1;

		setupRootNode(nodes[0], bounds);

		construct(nodes, bounds, bounds, localgeom, globalgeom, nodes, firstFreeElementInNodes, 0, out);

		assert(out.innerNodes = innerNodeCount);
		assert(out.leafNodes = leafNodeCount);
		
		assert(firstFreeElementInNodes == 2*globalgeom.size() -1);
	}

protected:
	// implemented by the deriving classes SingleSlabHierarchy and BoundingVolumeHierarchy. (see below this class)
	virtual void setupRootNode(Node& node, const AABBox &bounds) = 0;
	virtual AABBox setNodeVolume(Node& node, const AABBox& parentBounds, const AABBox& bounds, SceneConstructionDetails& out) = 0;
	
private:
	unsigned long innerNodeCount;
	unsigned long leafNodeCount;

	/*
		construct the hierarchy.
		in pseudocode:

		set volume/slab of current node
		if only one triangle 
		then 
			set leaf node with triangle index
		else
			get two free slots from the nodes array for childs
			set inner node with childIDs
			split triangles
			compute bounds of the two triangle groups
			call construct on both childs with new triangle groups ant their bounds
	*/
	void construct(
		Node* node,										// node to construct
		const AABBox &parentNodeBounds,					// (approximated) bounds of parent node
		const AABBox &nodeGeomBounds,					// bounds of triangles for node
		const std::vector<x_node_child_id_t> &nodegeom,		// triangle indices for node
		std::vector<Triangle> &treegeom,				// all triangles in the scene
		Node* nodes,									// nodes array
		unsigned long& firstFreeElementInNodes,			// first free slot in nodes array
		unsigned int depth,							// for tree height computation
		SceneConstructionDetails& out					// for debug and tests
	)
	{
		if(depth > out.height) out.height = depth;

		// set volume/slab of current node
		AABBox nodeBounds = setNodeVolume(*node, parentNodeBounds, nodeGeomBounds, out);
		assert ( ( nodeGeomBounds.surfaceArea() - nodeBounds.surfaceArea() ) < 0.00001f );

		assert(!nodegeom.empty());
		if (nodegeom.size() == 1)
		{
			// leaf node
			++leafNodeCount;

			// set leaf node with triangle index
			node->setLeaf(nodegeom[0]);
			assert(node->isLeaf());
			assert(node->getGeomIndex() == nodegeom[0]);
			return;
		}

		// inner node
		++innerNodeCount;

		// get two free slots from the nodes array for childs
		x_node_child_id_t childsId = firstFreeElementInNodes;
		firstFreeElementInNodes += 2;

		// set inner node with childIDs
		node->setInner(childsId);
		assert(node->getChildId() == childsId);
		assert(!node->isLeaf());

		// compute half volumes
		AABBox nHalfVolume ( nodeGeomBounds ), fHalfVolume ( nodeGeomBounds );
		vec diagonal = nodeGeomBounds.max - nodeGeomBounds.min;
		if ( diagonal.x > diagonal.y )
		{
			if ( diagonal.x > diagonal.z )
			{
				nHalfVolume.max.x = fHalfVolume.min.x = ( nodeGeomBounds.min.x + nodeGeomBounds.max.x ) *0.5f;
				#ifdef TRAVERSE_ORDERED
					node->setSplitAxis(Node::AXIS_X);
					assert(node->getSplitAxis() == Node::AXIS_X);
				#endif
			}
			else
			{
				nHalfVolume.max.z = fHalfVolume.min.z = ( nodeGeomBounds.min.z + nodeGeomBounds.max.z ) *0.5f;
				#ifdef TRAVERSE_ORDERED
					node->setSplitAxis(Node::AXIS_Z);
					assert(node->getSplitAxis() == Node::AXIS_Z);
				#endif
			}
		}
		else
		{
			if ( diagonal.y > diagonal.z )
			{
				nHalfVolume.max.y = fHalfVolume.min.y = ( nodeGeomBounds.min.y + nodeGeomBounds.max.y ) *0.5f;
				#ifdef TRAVERSE_ORDERED
					node->setSplitAxis(Node::AXIS_Y);
					assert(node->getSplitAxis() == Node::AXIS_Y);
				#endif
			}
			else
			{
				nHalfVolume.max.z = fHalfVolume.min.z = ( nodeGeomBounds.min.z + nodeGeomBounds.max.z ) *0.5f;
				#ifdef TRAVERSE_ORDERED
					node->setSplitAxis(Node::AXIS_Z);
					assert(node->getSplitAxis() == Node::AXIS_Z);
				#endif
			}
		}

		std::vector<x_node_child_id_t> nGeo;
		std::vector<x_node_child_id_t> fGeo;
		
		// split triangles and compute bounds of the two triangle groups
		AABBox nBounds, fBounds;
		for ( unsigned int i = 0; i < nodegeom.size(); ++i )
		{
			const AABBox &geobounds = treegeom[nodegeom[i]].getBounds();
			vec midpoint = ( geobounds.min + geobounds.max ) *0.5f;
			if ( nHalfVolume.contains ( midpoint ) )
			{
				nGeo.push_back ( nodegeom[i] );
				nBounds.extend ( geobounds );
			}
			else
			{
				fGeo.push_back ( nodegeom[i] );
				fBounds.extend ( geobounds );
			}
		}
		// if all triangles lie on one side of the split plane then split the group in half (not spatial)
		if ( nGeo.empty() || fGeo.empty() )
		{
			nBounds.clear();
			fBounds.clear();
			nGeo.clear();
			fGeo.clear();

			unsigned int mid = nodegeom.size() / 2;
			for ( unsigned int i=0; i<mid; ++i )
			{
				const AABBox &geobounds = treegeom[nodegeom[i]].getBounds();
				nGeo.push_back ( nodegeom[i] );
				nBounds.extend ( geobounds );
			}
			for ( unsigned int i=mid; i<nodegeom.size(); ++i )
			{
				const AABBox &geobounds = treegeom[nodegeom[i]].getBounds();
				fGeo.push_back ( nodegeom[i] );
				fBounds.extend ( geobounds );
			}
		}

		assert(!nGeo.empty());
		assert(!fGeo.empty());

		construct(&nodes[childsId],   nodeBounds, nBounds, nGeo, treegeom, nodes, firstFreeElementInNodes, depth+1, out);
		construct(&nodes[childsId+1], nodeBounds, fBounds, fGeo, treegeom, nodes, firstFreeElementInNodes, depth+1, out);
	}
};

class SingleSlabHierarchySpatialMedianCut : public XHierarchySpatialMedianCut<SSHNode>
{
protected:
	virtual void setupRootNode(SSHNode& node, const AABBox &bounds);
	virtual AABBox setNodeVolume(SSHNode& node, const AABBox& parentBounds, const AABBox& bounds, SceneConstructionDetails& out);
};

class BoundingVolumeHierarchySpatialMedianCut : public XHierarchySpatialMedianCut<BVHNode>
{
protected:
	virtual void setupRootNode(BVHNode& node, const AABBox &bounds);
	virtual AABBox setNodeVolume(BVHNode& node, const AABBox& parentBounds, const AABBox& bounds, SceneConstructionDetails& out);
};

#endif

