#ifndef MATERIAL_H
#define MATERIAL_H

#include "vecmath.h"
#include "Light.hpp"
#include "Ray.hpp"
#include <vector>

//#define ITERATIVE_SHADOWS

class Material : public SIMDmemAligned
{
protected:
	bool receiveShadows;
	bool hasReflection;
	bool hasRefraction;

	// determines angle of refraction ray and amount of reflected vs. refracted
	float refractionIndex;
	qfloat qrefractionIndex;

	// the amount of reflection vs. material color
	vec reflectionFilter;
	qvec qreflectionFilter;

	// refraction result is multiplied with refractionFilter. this represents the object inside's color.
	vec refractionFilter;
	qvec qrefractionFilter;

public:
	/// <refraction> index represents the density.
	/// <reflection> is the amount of blending between reflection and phong shading result
	/// <refractionFilter> is multiplied with refraction raytrace result. it represents the object inside's color.
	Material(bool hasReflection, bool hasRefraction, float refractionIndex, const vec& reflectionFilter, const vec& refractionFilter)
		: receiveShadows(!hasReflection && !hasRefraction), hasReflection(hasReflection), hasRefraction(hasRefraction), refractionIndex(refractionIndex), qrefractionIndex(refractionIndex), reflectionFilter(reflectionFilter), qreflectionFilter(reflectionFilter), refractionFilter(refractionFilter), qrefractionFilter(refractionFilter) {}

	/// applies reflection/refraction and adds the color to destination
	void endShade(vec& destination, const vec& surfaceColor, const vec& specularColor, const vec& contribution, int level, const vec& position, const vec& normal, const vec& toEye);
	void endShade(const quad<vec*>& destination, const qvec& surfaceColor, const qvec& specularColor, const qvec& contribution, int level, const qvec& position, const qvec& normal, const qvec& toEye);

	virtual void shade(vec& destination, const vec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray, int index) = 0;
	virtual void shade(const quad<vec*>& destination, const qvec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray) = 0;
};

#endif
