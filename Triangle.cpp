#include "Triangle.hpp"
#include "vecmath.h"

#include <iostream>
using namespace std;

unsigned Triangle::intersectionTestsPerformed;

void Triangle::intersect(PackedRay &ray)
{
	Triangle::intersectionTestsPerformed++;

	//if((dot(ray.dir, qvec(vec(na))) < qfloat(0.0f)).allTrue()) return;

	const qvec edge1(edge_ab);
	const qvec edge2(edge_ac);
	
	const qvec vecP = cross(ray.dir, edge2);
	
	const qfloat det = dot(edge1, vecP);
	
	qfloat qepsilon;
	qepsilon = 1E-8;
	qmask hit;
	hit = abs(det) >= qepsilon;
	if (hit.allFalse()) return;
	
	//const qfloat inv_det = qfloat(1.0f)/det;
	const qfloat inv_det = rcp(det);
	
	const qvec vecT = ray.origin - a;
	qfloat lambda = dot(vecT, vecP);
	lambda *= inv_det;
	
	qfloat zero(0.0f);
	qfloat one(1.0f);
	hit &= (zero <= lambda) & (lambda <= one);
	if (hit.allFalse()) return;
	
	const qvec vecQ = cross(vecT, edge1);
	qfloat mue = dot(ray.dir, vecQ);
	mue *= inv_det;
	
	hit &= (zero <= mue) & (mue+lambda <= one);
	if (hit.allFalse()) return;
	
	qfloat f = dot(edge2, vecQ);
	f *= inv_det;
	hit &= (zero < f) & (f <= ray.t);
	if (hit.allFalse()) return;
	
	ray.u.condAssign(hit, lambda, ray.u);
	ray.v.condAssign(hit, mue, ray.v);
	ray.t.condAssign(hit, f, ray.t);
	ray.hit.condAssign(hit, this, ray.hit);
}

