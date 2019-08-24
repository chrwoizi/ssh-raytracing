#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "Light.hpp"

#undef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))

class PointLight : public Light
{
protected:
	vec position;
	float linearAttenuation;
	float squaredAttenuation;
	qfloat qlinearAttenuation;
	qfloat qsquaredAttenuation;

public:
	PointLight(const vec& color, const vec& position, float linearAttenuation = 0.0f, float squaredAttenuation = 0.0f) 
		: Light(color), position(position), linearAttenuation(linearAttenuation), squaredAttenuation(squaredAttenuation),
					qlinearAttenuation(linearAttenuation), qsquaredAttenuation(squaredAttenuation) {}

	/// returns the light color and intensity at the given point in space.
	/// color and intensity are given as one vector. i.e. (0.1, 0, 0) is a dark red.
	/// result contains attenuation.
	virtual vec getColor(const vec& position, const vec& samplePosition) 
	{ 
		vec d = samplePosition - position;
		float distance = length(d);
		float attenuation = MAX(0.0f, 1.0f - distance*(linearAttenuation + distance*squaredAttenuation));
		return color * attenuation; 
	}
	virtual qvec getColor(const qvec& position, const qvec& samplePosition) 
	{ 
		qvec d = samplePosition - position;
		qfloat distance = length(d);
		qfloat attenuation = qfloat(1.0f) - distance*qlinearAttenuation + distance*qsquaredAttenuation;
		attenuation[0] = MAX(0.f, attenuation[0]);
		attenuation[1] = MAX(0.f, attenuation[1]);
		attenuation[2] = MAX(0.f, attenuation[2]);
		attenuation[3] = MAX(0.f, attenuation[3]);
		return qcolor * attenuation; 
	}

	/// returns the light direction for a given point in space.
	/// the returned vector points from the object to the light source.
	virtual vec getDirection(const vec& position, const vec& samplePosition) 
	{ 
		vec result(this->position - position);
		normalize(result);
		return result; 
	}
	virtual qvec getDirection(const qvec& position, const qvec& samplePosition) 
	{ 
		qvec result(samplePosition - position);
		normalize(result);
		return result; 
	}

	/// returns the distance between the light and a given point in space.
	virtual float getDistance(const vec& position, const vec& samplePosition) 
	{ 
		return length(samplePosition - position); 
	}
	virtual qfloat getDistance(const qvec& position, const qvec& samplePosition) 
	{ 
		return length(samplePosition - position); 
	}
	
	/// returns the light position of a sample.
	virtual vec getSamplePosition(int sample)
	{
		return this->position;
	}
};

#endif