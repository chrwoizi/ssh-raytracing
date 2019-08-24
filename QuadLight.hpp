#ifndef QUADLIGHT_HPP
#define QUADLIGHT_HPP

#include "Light.hpp"
#include <stdlib.h>
#include <math.h>

#undef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))

class QuadLight : public Light
{
protected:
	vec position;
	qvec qposition;
	
	vec normal;
	vec binormal;
	vec tangent;

	float width;
	float height;
	
	float linearAttenuation;
	float squaredAttenuation;
	qfloat qlinearAttenuation;
	qfloat qsquaredAttenuation;

public:
	QuadLight(const vec& color, const vec& position, float width, float height, const vec& normal, const vec& binormal, float linearAttenuation = 0.0f, float squaredAttenuation = 0.0f) 
		: Light(color), position(position), qposition(position), width(width), height(height), linearAttenuation(linearAttenuation), squaredAttenuation(squaredAttenuation),
		qlinearAttenuation(linearAttenuation), qsquaredAttenuation(squaredAttenuation)
	{
		this->normal = normal;
		normalize(this->normal);

		this->tangent = cross(this->normal, binormal);
		this->binormal = cross(this->normal, this->tangent);
	}

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
		vec result(samplePosition - position);
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

	/// gets the number of samples required to produce a good result. 
	/// i.e. returns 1 for point light sources and 1000 for area light sources of a certain size.
	virtual int getSampleCount() { return 100; }

	/// returns the light position of a sample.
	virtual vec getSamplePosition(int sample) 
	{
		assert(width == height);
		static int samplesPerDimension = (int)sqrtf(getSampleCount());

		// compute x and y
		float sampleX = width * float(sample % samplesPerDimension) / float(samplesPerDimension);
		float sampleY = height * float(sample / samplesPerDimension) / float(samplesPerDimension);
		
		return vec(position + sampleX*binormal + sampleY*tangent);
	}
};

#endif