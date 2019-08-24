#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "Scene.hpp"


class kdTreeConstructionStrategy;


class kdTree : public Scene
{
  public:
	kdTree(kdTreeConstructionStrategy *conStrat)
	: root(NULL), conStrat(conStrat)
	{}

	~kdTree();
	
	virtual SceneConstructionDetails construct(std::vector<Triangle>* geometries);
	virtual IntersectDetails intersect(PackedRay&);
	
	virtual const AABBox& getBounds() const { return bounds; }
	
	virtual unsigned long getComputedMemoryUsage() const
	{
		return 0;	// not implemented
	}
	
//  protected:
	struct kdNode
	{
		enum FLAGS {
			X_FLAG = 0,		// b00
			Y_FLAG = 1,		// b01
			Z_FLAG = 2,		// b10
			LEAF_FLAG = 3	// b11
		};
		#define FLAG_MASK  0x00000003
		
		union {
			kdNode *child;
			unsigned long flags;
		};
		union {
			float split;
			unsigned int geoindex;
		};
	};
	
  protected:
	kdNode *root;
	AABBox bounds;
	std::vector<Triangle>* geo;
	kdTreeConstructionStrategy *conStrat;
	
	void traverseFrontToBack(PackedRay &ray, qfloat& t0, qfloat& t1, const qmask reverse[3], IntersectDetails& out);
	void traverseDepthFirst(PackedRay &ray, qfloat& t0, qfloat& t1, const qmask reverse[3], IntersectDetails& out);
	
	kdNode* pack(kdNode *node, kdNode *nodeArrayPos);
};


/*
 * strategy pattern for tree construction algorithms
 */
class kdTreeConstructionStrategy
{
  public:
	virtual ~kdTreeConstructionStrategy() {}
	virtual kdTree::kdNode* construct(std::vector<Triangle>* globalgeom,
									  std::vector<Triangle*>& localgeom,
									  const AABBox &bounds,
									  unsigned int &nodecount) = 0;
};

#endif

