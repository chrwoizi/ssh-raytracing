#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vecmath.h"

class Light : public SIMDmemAligned
{
protected:
	vec color;
	qvec qcolor;

public:
	Light(vec color) : color(color), qcolor(color) {}

	/// returns the light color and intensity at the given point in space.
	/// color and intensity are given as one vector. i.e. (0.1, 0, 0) is a dark red.
	/// result contains attenuation.
	virtual vec getColor(const vec& position, const vec& samplePosition) { return color; }
	virtual qvec getColor(const qvec& position, const qvec& samplePosition) { return qcolor; }

	/// returns the light direction for a given point in space.
	/// the returned vector points from the object to the light source.
	virtual vec getDirection(const vec& position, const vec& samplePosition) = 0;
	virtual qvec getDirection(const qvec& position, const qvec& samplePosition) = 0;

	/// returns the distance between the light and a given point in space.
	virtual float getDistance(const vec& position, const vec& samplePosition) = 0;
	virtual qfloat getDistance(const qvec& position, const qvec& samplePosition) = 0;

	/// gets the number of samples required to produce a good result. 
	/// i.e. returns 1 for point light sources and 1000 for area light sources of a certain size.
	virtual int getSampleCount() { return 1; }

	/// returns the light position of a sample.
	virtual vec getSamplePosition(int sample) = 0; 
};

#endif