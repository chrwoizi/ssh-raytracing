#ifndef EYELIGHTCOLORMATERIAL_H
#define EYELIGHTCOLORMATERIAL_H

#include "EyelightMaterial.hpp"

class EyeLightColorMaterial : public EyelightMaterial
{
private:
	vec color;

public:
	/// <refraction> index represents the density.
	/// <reflection> is the amount of blending between reflection and phong shading result
	/// <refractionFilter> is multiplied with refraction raytrace result. it represents the object inside's color.
	EyeLightColorMaterial(const vec& color, bool hasReflection = false, bool hasRefraction = false, float refractionIndex = 0, const vec& reflectionFilter = vec(1,1,1), const vec& refractionFilter = vec(1,1,1)) 
		: EyelightMaterial(hasReflection, hasRefraction, refractionIndex, reflectionFilter, refractionFilter), color(color) {}

	/// returns the color at texture coordinates
	virtual vec getColor(const vec& textureCoordinates) { return color; }
	virtual qvec getColor(const qvec& textureCoordinates) { return qvec(color); }
};

#endif
