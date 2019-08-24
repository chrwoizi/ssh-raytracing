#ifndef XHIERARCHY_HPP
#define XHIERARCHY_HPP

#include <string.h>

#include "MultiThreading.hpp"
#include "XHierarchyConfig.hpp"

#include "SSHNode.hpp"
#include "BVHNode.hpp"

#include "Triangle.hpp"
#include "Scene.hpp"

/*
* strategy pattern for hierarchy construction algorithms
*/
template<typename Node>
class XHierarchyConstructionStrategy
{
public:
	virtual ~XHierarchyConstructionStrategy() {}
	virtual void construct(
		std::vector<Triangle> &globalgeom,
		const AABBox &bounds,
		Node* nodes,
		SceneConstructionDetails& out
	) = 0;

protected:
	unsigned int innerNodeCount;
	unsigned int leafNodeCount;
};

template<typename Node>
class XHierarchy : public Scene
{
public:
	XHierarchy(XHierarchyConstructionStrategy<Node> *conStrat) : conStrat(conStrat), root(NULL) {}
	~XHierarchy()
	{
		delete conStrat;
		delete[] root;
	}

	virtual unsigned long getComputedMemoryUsage() const
	{
		return Node::memSize * nodeCount;
	}

	virtual const AABBox& getBounds() const { return bounds; }

	virtual IntersectDetails intersect(PackedRay& ray)
	{
		IntersectDetails result;
		result.rayNodeIntersections = 0;

		qfloat tnear = 0.0f;
		qfloat tfar = ray.t;

		bounds.clip(ray, tnear, tfar);

		if ((tnear > tfar).allTrue())
		{
			return result;	// all rays miss bounds
		}

		qmask reverse[3];
		reverse[0] = ray.dirrcp.x < 0.0f;
		reverse[1] = ray.dirrcp.y < 0.0f;
		reverse[2] = ray.dirrcp.z < 0.0f;

		#ifdef TRAVERSE_ITERATIVE
			traverse_iterative(ray, root, tnear, tfar, reverse, result);
		#else
			traverse_recursive(ray, root, root, tnear, tfar, reverse, result);
		#endif

		return result;
	}

	virtual SceneConstructionDetails construct(std::vector<Triangle>* geometries)
	{
		assert(geometries);
		this->triangles = geometries;

		// compute bounds
		bounds.clear();
		for (unsigned int i = 0; i < geometries->size(); ++i)
		{
			bounds.extend( (*geometries)[i].getBounds() );
		}

		// all leaf nodes contain exactly 1 geometry -> leaf node count == geomcount, inner node count == geomcount-1
		nodeCount = 2*geometries->size() -1;
		
		if(root) delete[] root;
		root = new Node[nodeCount];

		SceneConstructionDetails result;
		result.height = 0;

		conStrat->construct(*triangles, bounds, root, result);

		#ifdef TRAVERSE_ITERATIVE
			#ifdef MULTITHREADING
				for(int i = 0; i < THREAD_COUNT; ++i)
				{
					remainingNodes[i].reserve(result.height);
				}
			#else
				remainingNodes.reserve(result.height);
			#endif
		#endif

		return result;
	}

private:
	template<typename T>
	class Stack
	{
	private:
		T* data;
		unsigned long currentFree;
		unsigned long size;
		
	public:
		Stack(size_t size = 1) : size(size)
		{
			data = new T[size];
			currentFree = 0;
		}

		inline void reserve(unsigned long count)
		{
			assert(size <= count);
			size = count;
			T* newData = new T[size];
			if(currentFree > 0) memcpy(newData, data, sizeof(T)*currentFree);
			delete[] data;
			data = newData;
		}

		// returned reference is valid until next pop or reserve
		inline T& push()
		{
			assert(currentFree < size);
			return data[currentFree++];
		}

		// returned reference is valid until next push or reserve
		inline T& pop()
		{
			assert(currentFree != 0);
			return data[--currentFree];
		}

		inline void clear()
		{
			currentFree = 0;
		}

		inline bool empty()
		{
			return currentFree == 0;
		}
	};

protected:
	// for iterative traversal
	struct StackData : public SIMDmemAligned
	{
		Node* node;
		qfloat t_near;
		qfloat t_far;
	};
	#ifdef MULTITHREADING
		Stack<StackData> remainingNodes[THREAD_COUNT];
	#else
		Stack<StackData> remainingNodes;
	#endif

	XHierarchyConstructionStrategy<Node> *conStrat;
	Node *root;
	unsigned long nodeCount;
	AABBox bounds;
	std::vector<Triangle>* triangles;
	
	virtual void updateActiveRaySegment(const PackedRay &ray, const qmask reverse[3], const Node *bounds, qfloat& t_near, qfloat& t_far) = 0;

private:
	void traverse_iterative(
		PackedRay& ray,
		Node* rootNode,
		qfloat& t_near_r,
		qfloat& t_far_r,
		const qmask reverse[3],
		IntersectDetails& out
	)
	{
		// qfloat alignment
		qfloat t_near = t_near_r;
		qfloat t_far = t_far_r;

		#ifdef MULTITHREADING
			Stack<StackData>& remainingNodes = this->remainingNodes[omp_get_thread_num()];
		#else
			Stack<StackData>& remainingNodes = this->remainingNodes;
		#endif
		
		remainingNodes.clear();

		Node* currentNode = rootNode;

		// traverse near nodes and push far nodes onto stack
		while(true)
		{
			updateActiveRaySegment(ray, reverse, currentNode, t_near, t_far);
			++out.rayNodeIntersections;

			if (
				(t_near > t_far).allTrue()
				#ifdef TRAVERSE_ORDERED
					|| (t_near > ray.t).allTrue()
				#endif
			)
			{
				// current node not hit. try node from stack
				
				if(remainingNodes.empty()) return;

				StackData& sd = remainingNodes.pop();
					
				currentNode = sd.node;
				t_near = sd.t_near;
				t_far = sd.t_far;

				continue;
			}

			if (currentNode->isLeaf())
			{
				// leaf node -> intersect with geometry
				(*this->triangles)[currentNode->getGeomIndex()].intersect(ray);

				// traverse node from stack
				
				if(remainingNodes.empty()) return;

				StackData& sd = remainingNodes.pop();

				currentNode = sd.node;
				t_near = sd.t_near;
				t_far = sd.t_far;

				continue;
			}

			// inner node

			// push new stack element
			StackData& sd = remainingNodes.push();

			// write directly to stack through reference
			sd.t_near = t_near;
			sd.t_far = t_far;

			x_node_child_id_t child = currentNode->getChildId();

			#ifdef TRAVERSE_ORDERED
				// ordered traversal. traverse near node first.
				if(reverse[currentNode->getSplitAxis()].mask())
				{
					currentNode = rootNode + child+1;
					sd.node = rootNode + child;
				}
				else
				{
					currentNode = rootNode + child;
					sd.node = rootNode + child+1;
				}
			#else
				currentNode = rootNode + child;
				sd.node = rootNode + child+1;
			#endif
		}
	}

	void traverse_recursive(
		PackedRay& ray,
		Node* node,
		Node* rootNode,
		qfloat& t_near_r,
		qfloat& t_far_r,
		const qmask reverse[3],
		IntersectDetails& out
	)
	{
		// qfloat alignment
		qfloat t_near = t_near_r;
		qfloat t_far = t_far_r;

		updateActiveRaySegment(ray, reverse, node, t_near, t_far);
		++out.rayNodeIntersections;
		
		if (
			(t_near > t_far).allTrue()
			#ifdef TRAVERSE_ORDERED
				|| (t_near > ray.t).allTrue()
			#endif
		)
		{
			return;
		}

		if (node->isLeaf())
		{
			// leaf node
			(*this->triangles)[node->getGeomIndex()].intersect(ray);
			
			return;
		}

		// inner node
			
		x_node_child_id_t child = node->getChildId();

		#ifdef TRAVERSE_ORDERED
			// ordered traversal for ray packages with uniform direction signs. traverse near node first.
			if(reverse[node->getSplitAxis()].allTrue())
			{
				traverse_recursive(ray, rootNode + child + 1, rootNode, t_near, t_far, reverse, out);
				traverse_recursive(ray, rootNode + child, rootNode, t_near, t_far, reverse, out);
			}
			else
			{
				traverse_recursive(ray, rootNode + child, rootNode, t_near, t_far, reverse, out);
				traverse_recursive(ray, rootNode + child + 1, rootNode, t_near, t_far, reverse, out);
			}
		#else
			traverse_recursive(ray, rootNode + child, rootNode, t_near, t_far, reverse, out);
			traverse_recursive(ray, rootNode + child+1, rootNode, t_near, t_far, reverse, out);
		#endif
	}
};

class SingleSlabHierarchy : public XHierarchy<SSHNode>
{
public:
	SingleSlabHierarchy(XHierarchyConstructionStrategy<SSHNode>* conStrat) : XHierarchy<SSHNode>(conStrat) {}
protected:
	virtual void updateActiveRaySegment(const PackedRay &ray, const qmask reverse[3], const SSHNode* node, qfloat& t_near, qfloat& t_far);
};

class BoundingVolumeHierarchy : public XHierarchy<BVHNode>
{
public:
	BoundingVolumeHierarchy(XHierarchyConstructionStrategy<BVHNode>* conStrat) : XHierarchy<BVHNode>(conStrat) {}
protected:
	virtual void updateActiveRaySegment(const PackedRay &ray, const qmask reverse[3], const BVHNode *node, qfloat& t_near, qfloat& t_far);
};

#endif

