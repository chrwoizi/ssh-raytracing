#include <cmath>

#include "Camera.hpp"
#include "string.h"	// NULL def

#ifndef M_PI
#define M_PI 3.141592
#endif

Camera::Camera(unsigned int xRes, unsigned int yRes, float fovy)
: xRes(xRes), yRes(yRes), fovy(fovy), pos(vec(0.0f,0.0f,0.0f)), dir(0.0f,0.0f,-1.0f), up(0.0f,1.0f,0.0f)
{	
	float aspect = static_cast<float>(xRes) / static_cast<float>(yRes);
	float yAxisLength = tanf((M_PI * fovy * 0.5f) / 180.0f) * length(dir);
	
	xAxis = cross(dir, up);
	normalize(xAxis);
	xAxis *= yAxisLength * aspect;
	
	yAxis = cross(xAxis, dir);
	normalize(yAxis);
	yAxis *= yAxisLength;
	
	origin = dir - yAxis - xAxis;
	yAxis /= static_cast<float>(yRes) / 2.0f;
	xAxis /= static_cast<float>(xRes) / 2.0f;
	
	qvec offset;
	offset.x[0] = 0.0f    + 0.0f;
	offset.y[0] = 0.0f    + 0.0f;
	offset.z[0] = 0.0f    + 0.0f;
	offset.x[1] = xAxis.x + 0.0f;
	offset.y[1] = xAxis.y + 0.0f;
	offset.z[1] = xAxis.z + 0.0f;
	offset.x[2] = 0.0f    + yAxis.x;
	offset.y[2] = 0.0f    + yAxis.y;
	offset.z[2] = 0.0f    + yAxis.z;
	offset.x[3] = xAxis.x + yAxis.x;
	offset.y[3] = xAxis.y + yAxis.y;
	offset.z[3] = xAxis.z + yAxis.z;
	
	origin += offset;
}

Camera::Camera(unsigned int xRes, unsigned int yRes, float fovy,
				vec pos, vec dir, vec up)
: xRes(xRes), yRes(yRes), fovy(fovy), pos(pos), dir(dir), up(up)
{
	float aspect = static_cast<float>(xRes) / static_cast<float>(yRes);
	float yAxisLength = tanf((M_PI * fovy * 0.5f) / 180.0f) * length(dir);
	
	xAxis = cross(dir, up);
	normalize(xAxis);
	xAxis *= yAxisLength * aspect;
	
	yAxis = cross(xAxis, dir);
	normalize(yAxis);
	yAxis *= yAxisLength;
	
	origin = dir - yAxis - xAxis;
	yAxis /= static_cast<float>(yRes) / 2.0f;
	xAxis /= static_cast<float>(xRes) / 2.0f;
	
	qvec offset;
	offset.x[0] = 0.0f    + 0.0f;
	offset.y[0] = 0.0f    + 0.0f;
	offset.z[0] = 0.0f    + 0.0f;
	offset.x[1] = xAxis.x + 0.0f;
	offset.y[1] = xAxis.y + 0.0f;
	offset.z[1] = xAxis.z + 0.0f;
	offset.x[2] = 0.0f    + yAxis.x;
	offset.y[2] = 0.0f    + yAxis.y;
	offset.z[2] = 0.0f    + yAxis.z;
	offset.x[3] = xAxis.x + yAxis.x;
	offset.y[3] = xAxis.y + yAxis.y;
	offset.z[3] = xAxis.z + yAxis.z;
	
	origin += offset;
}

void Camera::set(vec pos, vec dir, vec up)
{
	this->pos = pos;
	this->dir = dir;
	this->up = up;

	float aspect = static_cast<float>(xRes) / static_cast<float>(yRes);
	float yAxisLength = tanf((M_PI * fovy * 0.5f) / 180.0f) * length(dir);
	
	xAxis = cross(dir, up);
	normalize(xAxis);
	xAxis *= yAxisLength * aspect;
	
	yAxis = cross(xAxis, dir);
	normalize(yAxis);
	yAxis *= yAxisLength;
	
	origin = dir - yAxis - xAxis;
	yAxis /= static_cast<float>(yRes) / 2.0f;
	xAxis /= static_cast<float>(xRes) / 2.0f;
	
	qvec offset;
	offset.x[0] = 0.0f    + 0.0f;
	offset.y[0] = 0.0f    + 0.0f;
	offset.z[0] = 0.0f    + 0.0f;
	offset.x[1] = xAxis.x + 0.0f;
	offset.y[1] = xAxis.y + 0.0f;
	offset.z[1] = xAxis.z + 0.0f;
	offset.x[2] = 0.0f    + yAxis.x;
	offset.y[2] = 0.0f    + yAxis.y;
	offset.z[2] = 0.0f    + yAxis.z;
	offset.x[3] = xAxis.x + yAxis.x;
	offset.y[3] = xAxis.y + yAxis.y;
	offset.z[3] = xAxis.z + yAxis.z;
	
	origin += offset;
}

void Camera::lookAt(const AABBox &bbox)
{
	float boxwidth = bbox.max.x - bbox.min.x;
	float boxheight = bbox.max.y - bbox.min.y;
	
	float boxaspect = boxwidth / boxheight;
	float aspect = static_cast<float>(xRes) / static_cast<float>(yRes);
	
	this->pos.x = (bbox.min.x + bbox.max.x)*0.5f;
	this->pos.y = (bbox.min.y + bbox.max.y)*0.5f;
	
	float dist;
	if (aspect >= boxaspect) {
		boxheight *= 1.05f;
		dist = (boxheight*0.5f)/tanf((0.5f*fovy*M_PI)/180.0f);
	} else {
		boxwidth *= 1.05f;
		dist = (boxwidth*0.5f)/tanf((aspect*0.5f*fovy*M_PI)/180.0f);
	}
	this->pos.z = bbox.max.z + dist;
	
	this->dir = vec(0.0f, 0.0f, -dist);
	this->up = vec(0.0f, 1.0f, 0.0f);
	
	// copied from constructor
	float yAxisLength = tanf((M_PI * 60.0f * 0.5f) / 180.0f) * length(dir);
	
	xAxis = cross(dir, up);
	normalize(xAxis);
	xAxis *= yAxisLength * aspect;
	
	yAxis = cross(xAxis, dir);
	normalize(yAxis);
	yAxis *= yAxisLength;
	
	origin = dir - yAxis - xAxis;
	yAxis /= static_cast<float>(yRes) / 2.0f;
	xAxis /= static_cast<float>(xRes) / 2.0f;
	
	qvec offset;
	offset.x[0] = 0.0f    + 0.0f;
	offset.y[0] = 0.0f    + 0.0f;
	offset.z[0] = 0.0f    + 0.0f;
	offset.x[1] = xAxis.x + 0.0f;
	offset.y[1] = xAxis.y + 0.0f;
	offset.z[1] = xAxis.z + 0.0f;
	offset.x[2] = 0.0f    + yAxis.x;
	offset.y[2] = 0.0f    + yAxis.y;
	offset.z[2] = 0.0f    + yAxis.z;
	offset.x[3] = xAxis.x + yAxis.x;
	offset.y[3] = xAxis.y + yAxis.y;
	offset.z[3] = xAxis.z + yAxis.z;
	
	origin += offset;
}

void Camera::getRays(PackedRay& ray, int x, int y)
{
	ray.origin = pos;
	ray.dir = origin + vec(x)*xAxis + vec(y)*yAxis;
	ray.dirrcp = qvec(qfloat(1.0f)) / ray.dir;
	ray.t = HUGE_VAL;
	ray.hit = NULL;
}

void Camera::move(vec value)
{
	pos += value;
}
