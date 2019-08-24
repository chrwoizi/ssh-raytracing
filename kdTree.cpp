#include <algorithm>
#include <cassert>
/*
#include "kdTree.hpp"
#include "vecmath.h"

#include "Triangle.hpp"


using namespace std;


kdTree::~kdTree()
{
	delete conStrat;
	delete[] root;
}


SceneConstructionDetails kdTree::construct(vector<Triangle>* geometries)
{
	assert(geometries);
	this->geo = geometries;

	// compute bounds
	bounds.clear();
	for (unsigned int i = 0; i < geometries->size(); ++i)
		bounds.extend( (*geometries)[i].getBounds() );
	
	vector<Triangle*> nodegeom(geometries->size());
	for(unsigned long i = 0; i < geometries->size(); ++i)
	{
		nodegeom[i] = &(*geometries)[i];
	}
	
	unsigned int nodecount;
	root = conStrat->construct(geometries, nodegeom, bounds, nodecount);
	
#if 1
	// pack nodes in contiguous memory
	kdNode *allNodes = new kdNode[nodecount];
	assert(reinterpret_cast<unsigned long>(allNodes) % 4 == 0);
	allNodes[0] = *root;
	pack(allNodes, allNodes + 1);
	root = allNodes;
#endif

	SceneConstructionDetails result;
	result.innerNodes = 0;
	result.leafNodes = 0;
	result.height = 0;
	return result;
}

kdTree::kdNode* kdTree::pack(kdNode *node, kdNode *nodeArrayPos)
{
	if ((node->flags & FLAG_MASK) != 0x3) {
		// inner node
		union {
			unsigned long flags;
			kdNode *ptr;
		} ptrAndFlags;
		ptrAndFlags.ptr = node->child;
		ptrAndFlags.flags &= ~FLAG_MASK;
		nodeArrayPos[0] = *ptrAndFlags.ptr;
		nodeArrayPos[1] = *(ptrAndFlags.ptr + 1);
		kdNode *nextNodeArrayPos = pack(nodeArrayPos, nodeArrayPos + 2);
		nextNodeArrayPos = pack(nodeArrayPos + 1, nextNodeArrayPos);
		
		unsigned long flags = node->flags & FLAG_MASK;
		delete ptrAndFlags.ptr;
		node->child = nodeArrayPos;
		node->flags |= flags;
		
		return nextNodeArrayPos;
		
	} else {
		// leaf node
		return nodeArrayPos;
	}
}

IntersectDetails kdTree::intersect(PackedRay &ray)
{
	static quad<Primitive*> qnull = NULL;

	qfloat t0 = 0.0f;
	qfloat t1 = ray.t;

	IntersectDetails result;
	result.rayNodeIntersections = 0;
	
	bounds.clip(ray, t0, t1);
	
	if ((t0 > t1).allTrue())
		return result;	// all rays miss bounds
	
	qmask reverse[3];
	reverse[0] = ray.dirrcp.x < 0.0f;
	reverse[1] = ray.dirrcp.y < 0.0f;
	reverse[2] = ray.dirrcp.z < 0.0f;
#if 0
	if ((reverse[0].allTrue() || reverse[0].allFalse())
		&& (reverse[1].allTrue() || reverse[1].allFalse())
		&& (reverse[2].allTrue() || reverse[2].allFalse()))
		// rays are coherend enough to allow for front to back tree traversal
		traverseFrontToBack(ray, t0, t1, reverse, result);
	else
		// fallback to depth first tree traversal
		traverseDepthFirst(ray, t0, t1, reverse, result);
#else
	//traverseFrontToBack(ray, t0, t1, reverse);
	traverseDepthFirst(ray, t0, t1, reverse, result);
#endif

	return result;
}

void kdTree::traverseFrontToBack(PackedRay &ray, qfloat& t0_r, qfloat& t1_r, const qmask reverse[3], IntersectDetails& out)
{
	// qfloat alignment
	qfloat t0 = t0_r;
	qfloat t1 = t1_r;

	struct nodeStackElement {
		kdNode *node;
		qfloat t0;
		qfloat t1;
	};
	nodeStackElement nodestack[20];
	unsigned int nodestackindex = 0;
	
	kdNode *node = root;
	
	while (true) {
		unsigned long flag = node->flags & FLAG_MASK;
		
		// leaf node
		if (flag == kdNode::LEAF_FLAG) {
			// restrict intersection test to active ray interval
			ray.t.condAssign(ray.hit == NULL, t1, ray.t);
			
			unsigned int endindex = node->geoindex + (node->flags >> 2);
			for (unsigned int i = node->geoindex; i < endindex; ++i)
				(*geo)[i].intersect(ray);
			
			if (((ray.hit != NULL).allTrue()) || (nodestackindex == 0)) break;
			
			nodestackindex--;
			
			//pop
			node = nodestack[nodestackindex].node;
			t0   = nodestack[nodestackindex].t0;
			t1   = nodestack[nodestackindex].t1;
			// restore active ray interval
			//ray.t.condAssign(ray.hit == NULL, t1, ray.t);
			
			continue;
		}
		
		// inner node
		qfloat d = (qfloat(node->split) - ray.origin[flag]) * ray.dirrcp[flag];
		
		qmask near_only;
		qmask far_only;
		
		near_only  = (d <= t0) & reverse[flag];
		near_only |= (d >= t1).andnot(reverse[flag]);
		
		far_only  = (d <= t0).andnot(reverse[flag]);
		far_only |= (d >= t1) & reverse[flag];
		
		union {
			unsigned long flags;
			kdNode *ptr;
		} ptrAndFlags;
		ptrAndFlags.ptr = node->child;
		ptrAndFlags.flags &= ~FLAG_MASK;
		
		if (near_only.allTrue()) {
			// traverse near child only
			node = ptrAndFlags.ptr;
			
		} else if (far_only.allTrue()) {
			// traverse far child only
			node = ptrAndFlags.ptr + 1;
		
		} else if (reverse[flag].allFalse()) {
			// traverse both childs:
			// push far child
			nodestack[nodestackindex].node = ptrAndFlags.ptr + 1;
			nodestack[nodestackindex].t0 =  d;
			nodestack[nodestackindex].t1 = t1;
			nodestackindex++;
			// continue with near child
			node = ptrAndFlags.ptr;
			t0 = t0;
			t1 =  d;
			
		} else if (reverse[flag].allTrue()) {
			// traverse both childs:
			// push near child
			nodestack[nodestackindex].node = ptrAndFlags.ptr;
			nodestack[nodestackindex].t0 =  d;
			nodestack[nodestackindex].t1 = t1;
			nodestackindex++;
			// continue with far child
			node = ptrAndFlags.ptr + 1;
			t0 = t0;
			t1 =  d;
			
		}
#ifndef NDEBUG
		else
			assert(false);
			// this point should not be reachable, because we should have
			// switched over to traverseDepthFirst() in intersect()
#endif
	}
}

void kdTree::traverseDepthFirst(PackedRay &ray, qfloat& t0_r, qfloat& t1_r, const qmask reverse[3], IntersectDetails& out)
{
	// qfloat alignment
	qfloat t0 = t0_r;
	qfloat t1 = t1_r;

	struct nodeStackElement {
		kdNode *node;
		qfloat t0;
		qfloat t1;
	};
	nodeStackElement nodestack[20];
	unsigned int nodestackindex = 0;
	
	kdNode *node = root;
	
	while (true) {
		unsigned long flag = node->flags & FLAG_MASK;
		
		// leaf node
		if (flag == kdNode::LEAF_FLAG) {
			unsigned int endindex = node->geoindex + (node->flags >> 2);
			for (unsigned int i = node->geoindex; i < endindex; ++i)
				(*geo)[i].intersect(ray);
			
			if (nodestackindex == 0) break;
			
			nodestackindex--;
			
			//pop
			node = nodestack[nodestackindex].node;
			t0   = nodestack[nodestackindex].t0;
			t1   = nodestack[nodestackindex].t1;
			
			continue;
		}
		
		// inner node
		qfloat d = (qfloat(node->split) - ray.origin[flag]) * ray.dirrcp[flag];
		
		qmask near_only;
		qmask far_only;
		
		near_only  = (d <= t0) & reverse[flag];
		near_only |= (d >= t1).andnot(reverse[flag]);
		
		far_only  = (d <= t0).andnot(reverse[flag]);
		far_only |= (d >= t1) & reverse[flag];
		
		union {
			unsigned long flags;
			kdNode *ptr;
		} ptrAndFlags;
		ptrAndFlags.ptr = node->child;
		ptrAndFlags.flags &= ~FLAG_MASK;
		
		if (near_only.allTrue()) {
			// traverse near child only
			node = ptrAndFlags.ptr;
			
		} else if (far_only.allTrue()) {
			// traverse far child only
			node = ptrAndFlags.ptr + 1;
		
		} else {
#if 1
			// traverse both childs:
			// push far child
			nodestack[nodestackindex].node = ptrAndFlags.ptr + 1;
			nodestack[nodestackindex].t0 =  d;
			nodestack[nodestackindex].t1 = t1;
			nodestackindex++;
			// continue with near child
			node = ptrAndFlags.ptr;
			t0 = t0;
			t1 =  d;
#else
			// traverse both childs:
			// push near child
			nodestack[nodestackindex].node = ptrAndFlags.ptr;
			nodestack[nodestackindex].t0 =  d;
			nodestack[nodestackindex].t1 = t1;
			nodestackindex++;
			// continue with far child
			node = ptrAndFlags.ptr + 1;
			t0 = t0;
			t1 =  d;
#endif
		}
	}
}
*/