#ifndef SIMPLESCENE_HPP
#define SIMPLESCENE_HPP

#include "Scene.hpp"

class SimpleScene : public Scene
{
  public:
	virtual ~SimpleScene()
	{
	}
	
	virtual SceneConstructionDetails construct(std::vector<Triangle>* geom) {
		geometries = geom;
		// compute bounds
		bounds.clear();
		for (unsigned int i = 0; i < geom->size(); ++i)
			bounds.extend((*geom)[i].getBounds());

		SceneConstructionDetails result;
		result.innerNodes = 0;
		result.leafNodes = 0;
		result.height = 0;
		return result;
	}
	
	virtual IntersectDetails intersect(PackedRay& ray) {
		for (unsigned int i = 0; i < geometries->size(); ++i)
			(*geometries)[i].intersect(ray);
		return IntersectDetails();
	}
	
	virtual const AABBox& getBounds() const { return bounds; }
	
	virtual unsigned long getComputedMemoryUsage() const
	{
		return 0;
	}
	
  private:
	std::vector<Triangle>* geometries;
	AABBox bounds;
};

#endif
