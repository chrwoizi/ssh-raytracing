#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "AABBox.hpp"
#include "Ray.hpp"
#include "vecmath.h"

class Camera : public SIMDmemAligned
{
  private:
	unsigned int xRes, yRes;
	float fovy;
	vec pos;
	vec dir;
	vec up;
	
	qvec origin; 
	vec yAxis, xAxis;
	
  public:
	Camera(unsigned int xRes, unsigned int yRes, float fovy);
	
	Camera(unsigned int xRes, unsigned int yRes, float fovy, vec pos, vec dir, vec up);

	float getFov() { return fovy; }

	void set(vec pos, vec dir, vec up);

	void move(vec value);
	
	void lookAt(const AABBox &box);
	
	void getRays(PackedRay&,int,int);
};

#endif
