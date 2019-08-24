#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include "Light.hpp"

class DirectionalLight : public Light
{
protected:
	vec ndirection;
	qvec qndirection;

public:
	DirectionalLight(const vec& color, const vec& direction) 
		: Light(color), ndirection(-direction)
	{
		normalize(this->ndirection);
		qndirection = qvec(this->ndirection);
	}

	/// returns the light direction for a given point in space.
	/// the returned vector points from the object to the light source.
	virtual vec getDirection(const vec& position, const vec& samplePosition) 
	{ 
		return ndirection; 
	}
	virtual qvec getDirection(const qvec& position, const qvec& samplePosition) 
	{ 
		return qndirection; 
	}

	/// returns the distance between the light and a given point in space.
	virtual float getDistance(const vec& position, const vec& samplePosition) 
	{ 
		return HUGE_VAL; 
	}
	virtual qfloat getDistance(const qvec& position, const qvec& samplePosition) 
	{ 
		return HUGE_VAL; 
	}
	
	/// returns the light position of a sample.
	virtual vec getSamplePosition(int sample)
	{
		return vec(0,0,0);
	}
};

#endif