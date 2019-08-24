#ifndef PHONGCOLORMATERIAL_H
#define PHONGCOLORMATERIAL_H

#include "PhongMaterial.hpp"

class PhongColorMaterial : public PhongMaterial
{
private:
	vec ambientColor;
	vec diffuseColor;
	vec specularColor;
	float shininess;

public:
	/// <refraction> index represents the density.
	/// <reflection> is the amount of blending between reflection and phong shading result
	/// <refractionFilter> is multiplied with refraction raytrace result. it represents the object inside's color.
	PhongColorMaterial(const vec& ambientColor, const vec& diffuseColor, const vec& specularColor, float shininess, bool hasReflection = false, bool hasRefraction = false, float refractionIndex = 0, const vec& reflectionFilter = vec(1,1,1), const vec& refractionFilter = vec(1,1,1)) 
		: PhongMaterial(hasReflection, hasRefraction, refractionIndex, reflectionFilter, refractionFilter), ambientColor(ambientColor), diffuseColor(diffuseColor), specularColor(specularColor), shininess(shininess) {}

	/// returns the ambient color at texture coordinates
	virtual vec getAmbientColor(const vec& textureCoordinates) { return ambientColor; }
	virtual qvec getAmbientColor(const qvec& textureCoordinates) { return qvec(ambientColor); }

	/// returns the diffuse color at texture coordinates
	virtual vec getDiffuseColor(const vec& textureCoordinates) { return diffuseColor; }
	virtual qvec getDiffuseColor(const qvec& textureCoordinates) { return qvec(diffuseColor); }

	/// returns the specular color at texture coordinates
	virtual vec getSpecularColor(const vec& textureCoordinates) { return specularColor; }
	virtual qvec getSpecularColor(const qvec& textureCoordinates) { return qvec(specularColor); }

	/// returns the shininess exponent at texture coordinates
	virtual float getShininess(const vec& textureCoordinates) { return shininess; }
	virtual qfloat getShininess(const qvec& textureCoordinates) { return qfloat(shininess); }
};

#endif
