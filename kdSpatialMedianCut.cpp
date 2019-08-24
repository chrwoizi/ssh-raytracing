/*#include <iostream>

#include "kdSpatialMedianCut.hpp"
#include "Triangle.hpp"


kdTree::kdNode* kdSpatialMedianCut::construct(std::vector<Triangle*> &treegeom,
										const std::vector<Triangle>* nodegeom,
										const AABBox &bounds,
										unsigned int &nodecount)
{
#ifndef NDEBUG
	std::cout << "kdTree bounds : min(" << bounds.min.x << "," <<  bounds.min.y << "," <<  bounds.min.z << ") max(" << bounds.max.x << "," <<  bounds.max.y << "," <<  bounds.max.z << ")" << std::endl;
#endif
	innerNodeCount = 0;
	leafNodeCount = 0;
	
	kdTree::kdNode *root = new kdTree::kdNode;
	construct(root, bounds, *nodegeom, treegeom, 21);
			
#ifndef NDEBUG
	std::cout << "kdTree with " << innerNodeCount << " inner nodes and " << leafNodeCount << " leaf nodes constructed" << std::endl;
#endif
	
	nodecount = innerNodeCount + leafNodeCount;
	
	return root;
}


void kdSpatialMedianCut::construct(kdTree::kdNode *node, const AABBox &bounds,
									const std::vector<Triangle> &nodegeom,
									std::vector<Triangle*> &treegeom,
									unsigned int depth)
{
	assert(reinterpret_cast<unsigned long>(node) % 4 == 0);
	
	// leaf node
	if (nodegeom.size() <= 3 || depth == 0) {
		leafNodeCount++;
		node->flags = (nodegeom.size() << 2) | kdTree::kdNode::LEAF_FLAG;
		if (nodegeom.size() > 0) {
			node->geoindex = treegeom.size();
			for (unsigned int i = 0; i < nodegeom.size(); ++i)
				treegeom.push_back( const_cast<Triangle*>(&nodegeom[i]) );
		}
		return;
	}
	
	// inner node
	innerNodeCount++;
	kdTree::kdNode *child = new kdTree::kdNode[2];
	node->child = child;
	
	// compute dominating axis
	AABBox nBounds(bounds), fBounds(bounds);
	vec diagonal = bounds.max - bounds.min;
	if (diagonal.x > diagonal.y) {
		if (diagonal.x > diagonal.z) {
			node->flags |= kdTree::kdNode::X_FLAG;
			node->split = nBounds.max.x = fBounds.min.x
				= (bounds.min.x + bounds.max.x)*0.5f;
		} else {
			node->flags |= kdTree::kdNode::Z_FLAG;
			node->split = nBounds.max.z = fBounds.min.z
				= (bounds.min.z + bounds.max.z)*0.5f;
		}
	} else {
		if (diagonal.y > diagonal.z) {
			node->flags |= kdTree::kdNode::Y_FLAG;
			node->split = nBounds.max.y = fBounds.min.y
				= (bounds.min.y + bounds.max.y)*0.5f;
		} else {
			node->flags |= kdTree::kdNode::Z_FLAG;
			node->split = nBounds.max.z = fBounds.min.z
				= (bounds.min.z + bounds.max.z)*0.5f;
		}
	}
	
	// collect geometries per child-nodes
	std::vector<Triangle*> nGeo;
	std::vector<Triangle*> fGeo;
	
	for (unsigned int i = 0; i < nodegeom.size(); ++i) {
		if (nBounds.overlaps( nodegeom[i].getBounds() ))
			nGeo.push_back( const_cast<Triangle*>(&nodegeom[i]) );
		if (fBounds.overlaps( nodegeom[i].getBounds() ))
			fGeo.push_back( const_cast<Triangle*>(&nodegeom[i]) );
	}
	
	construct(child,     nBounds, nGeo, treegeom, depth - 1);
	construct(child + 1, fBounds, fGeo, treegeom, depth - 1);
}*/

