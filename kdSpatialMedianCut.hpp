/*#ifndef KDSPATIALMEDIANCUT_HPP
#define KDSPATIALMEDIANCUT_HPP

#include "kdTree.hpp"

class kdSpatialMedianCut : public kdTreeConstructionStrategy
{
  public:
	virtual ~kdSpatialMedianCut() {}
	virtual kdTree::kdNode* construct(std::vector<Triangle*> &treegeom,
									  const std::vector<Triangle>* nodegeom,
									  const AABBox &bounds,
									  unsigned int &nodecount);
	
  private:
	void construct(kdTree::kdNode *node, const AABBox &bounds,
				   const std::vector<Triangle> &nodegeom,
				   std::vector<Triangle*> &treegeom,
				   unsigned int depth);
	
	unsigned int innerNodeCount;
	unsigned int leafNodeCount;
};

#endif*/

