#include <iostream>
#include <algorithm>
/*
#include "kdSurfaceAreaHeuristic.hpp"


kdTree::kdNode* kdSurfaceAreaHeuristic::construct(std::vector<Geometry*> &treegeom,
												const std::vector<Geometry*> &nodegeom,
												const AABBox &bounds,
												unsigned int &nodecount)
{
#ifndef NDEBUG
	std::cout << "kdTree bounds : min(" << bounds.min.x << "," <<  bounds.min.y << "," <<  bounds.min.z << ") max(" << bounds.max.x << "," <<  bounds.max.y << "," <<  bounds.max.z << ")" << std::endl;
#endif
	innerNodeCount = 0;
	leafNodeCount = 0;
	
	kdTree::kdNode *root = new kdTree::kdNode;
	
	vec extend = bounds.max - bounds.min;
	float cost = extend.x*extend.y + extend.y*extend.z + extend.x*extend.z;
	cost *= 2.0f * float(nodegeom.size());
	construct(root, bounds, nodegeom, treegeom, 21, cost);
			
#ifndef NDEBUG
	std::cout << "kdTree with " << innerNodeCount << " inner nodes and " << leafNodeCount << " leaf nodes constructed" << std::endl;
#endif
	
	nodecount = innerNodeCount + leafNodeCount;
	
	return root;
}


struct SplitPointCandidate {
	float pos;
	int begin;
	int end;
};

bool operator<(const struct SplitPointCandidate &lh, const struct SplitPointCandidate &rh)
{
	return lh.pos < rh.pos;
}


void kdSurfaceAreaHeuristic::construct(kdTree::kdNode *node, const AABBox &bounds,
									const std::vector<Geometry*> &nodegeom,
									std::vector<Geometry*> &treegeom,
									unsigned int depth, const float leafcost)
{
	assert(reinterpret_cast<unsigned long>(node) % 4 == 0);
	
	// leaf node
	if (nodegeom.size() <= 3 || depth == 0) {
		constructLeafNode(node, nodegeom, treegeom);
		///leafNodeCount++;
		//node->flags = (nodegeom.size() << 2) | kdTree::kdNode::LEAF_FLAG;
		//if (nodegeom.size() > 0) {
		//	node->geoindex = treegeom.size();
		//	for (unsigned int i = 0; i < nodegeom.size(); ++i)
		//		treegeom.push_back( nodegeom[i] );
		//}
		return;
	}
	
	// inner node
	innerNodeCount++;
	
	// find split point
	float mincost = 0.0f;			// pseudo init to suppress warning
	float minnearcost = 0.0f;		// pseudo init to suppress warning
	float minfarcost = 0.0f;		// pseudo init to suppress warning
	float splitpoint = 0.0f;		// pseudo init to suppress warning
	unsigned int splitaxis = 0;		// pseudo init to suppress warning
	
	vec extend = bounds.max - bounds.min;
	
	std::vector<struct SplitPointCandidate> candidates;
	candidates.reserve(2*nodegeom.size());
	
	// evaluate all axes
	for (unsigned int axis = 0; axis < kdTree::kdNode::LEAF_FLAG; ++axis) {
	
		// collect split point candidates on axis
		candidates.clear();
		int overlapcount = 0;
		for (unsigned int i = 0; i < nodegeom.size(); ++i) {
			struct SplitPointCandidate cand;
			// first candidate
			if (nodegeom[i]->getBounds().min[axis] > bounds.min[axis]) {
				cand.pos = nodegeom[i]->getBounds().min[axis];
				cand.begin = 1;
				cand.end = 0;
				candidates.push_back(cand);
			} else {
				overlapcount += 1;
			}
			// second candidate
			if (nodegeom[i]->getBounds().max[axis] < bounds.max[axis]) {
				cand.pos = nodegeom[i]->getBounds().max[axis];
				cand.begin = 0;
				cand.end = 1;
				candidates.push_back(cand);
			}
		}
		
		std::sort(candidates.begin(), candidates.end());
		
		// precompute subexpressions for surface area calculation
		float saconst = 2.0f;
		float savar = 0.0f;
		for (unsigned int a = 0; a < kdTree::kdNode::LEAF_FLAG; ++a) {
			if (a == axis) {
				++a;	// skip actual axis
			} else {
				saconst *= extend[a];
				savar += extend[a];
			}
		}
		savar *= 2.0f;
		
		// evaluate split point candidates
		float split = 0.0f;		// pseudo init to suppress warning
		float cost = 0.0f;		// pseudo init to suppress warning
		float nearcost = 0.0f;	// pseudo init to suppress warning
		float farcost = 0.0f;	// pseudo init to suppress warning
		int nearcount = 0;
		//int overlapcount = 0;
		int farcount = nodegeom.size();
		for (unsigned int i = 0; i < candidates.size();) {
			unsigned int I = i;
			float pos = candidates[i].pos;
			do {
				nearcount += candidates[i].end;
				overlapcount += candidates[i].begin - candidates[i].end;
				farcount -= candidates[i].begin;
				++i;
			} while ((i < candidates.size()) && (candidates[i].pos == pos));
			
			nearcost = saconst + (pos - bounds.min[axis])*savar;	// surface area
			nearcost *= float(nearcount + overlapcount);
			farcost = saconst + (bounds.max[axis] - pos)*savar;	// surface area
			farcost *= float(farcount + overlapcount);
			if (((nearcost + farcost) < cost) || (I == 0)) {
				cost = nearcost + farcost;
				split = pos;
			}
		}
		
		// compare best splitpoint on axis with best splitpoint so far
		if ((cost < mincost) || (axis == kdTree::kdNode::X_FLAG)) {
			mincost = cost;
			minnearcost = nearcost;
			minfarcost = farcost;
			splitpoint = split;
			splitaxis = axis;
		}
	}
	
	// free some memory
	candidates.resize(0);
	
#if 0
	if (mincost >= leafcost) {
		// leaf node
		constructLeafNode(node, nodegeom, treegeom);
	} else
#endif
	{
		// inner node
		kdTree::kdNode *child = new kdTree::kdNode[2];
		node->child = child;
		node->split = splitpoint;
		node->flags |= splitaxis;
		
		// collect geometries per child-nodes
		AABBox nBounds(bounds), fBounds(bounds);
		nBounds.max[splitaxis] = fBounds.min[splitaxis] = splitpoint;
		
		std::vector<Geometry*> nGeo;
		std::vector<Geometry*> fGeo;
		
		for (unsigned int i = 0; i < nodegeom.size(); ++i) {
			if (nBounds.overlaps( nodegeom[i]->getBounds() ))
				nGeo.push_back( nodegeom[i] );
			if (fBounds.overlaps( nodegeom[i]->getBounds() ))
				fGeo.push_back( nodegeom[i] );
		}
		
		construct(child,     nBounds, nGeo, treegeom, depth - 1, minnearcost);
		construct(child + 1, fBounds, fGeo, treegeom, depth - 1, minfarcost);
	}
}


void kdSurfaceAreaHeuristic::constructLeafNode(kdTree::kdNode *node,
										const std::vector<Geometry*> &nodegeom,
										std::vector<Geometry*> &treegeom)
{
	leafNodeCount++;
	node->flags = (nodegeom.size() << 2) | kdTree::kdNode::LEAF_FLAG;
	if (nodegeom.size() > 0) {
		node->geoindex = treegeom.size();
		for (unsigned int i = 0; i < nodegeom.size(); ++i)
			treegeom.push_back( nodegeom[i] );
	}
}

*/