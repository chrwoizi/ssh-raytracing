#ifndef KDSURFACEAREAHEURISTIC_HPP
#define KDSURFACEAREAHEURISTIC_HPP

#include "kdTree.hpp"


class kdSurfaceAreaHeuristic : public kdTreeConstructionStrategy
{
  public:
	virtual ~kdSurfaceAreaHeuristic() {}
	virtual kdTree::kdNode* construct(std::vector<Triangle> &treegeom,
					const std::vector<unsigned long> &nodegeom,
					const AABBox &bounds,
					unsigned int &nodecount);
	
  private:
	void construct(kdTree::kdNode *node, const AABBox &bounds,
				   const std::vector<unsigned long> &nodegeom,
				   std::vector<Triangle> &treegeom,
				   unsigned int depth, const float leafcost);
	void constructLeafNode(kdTree::kdNode *node, const std::vector<unsigned long> &nodegeom, std::vector<Triangle> &treegeom);
	
	unsigned int innerNodeCount;
	unsigned int leafNodeCount;
};

#endif

