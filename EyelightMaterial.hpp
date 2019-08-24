#ifndef EYELIGHTMATERIAL_HPP
#define EYELIGHTMATERIAL_HPP

#include "Material.hpp"

#define MAX(a,b) ((a)>(b)?(a):(b))

class EyelightMaterial : public Material
{
public:
	/// <refraction> index represents the density.
	/// <reflection> is the amount of blending between reflection and phong shading result
	/// <refractionFilter> is multiplied with refraction raytrace result. it represents the object inside's color.
	EyelightMaterial(bool hasReflection, bool hasRefraction, float refractionIndex, const vec& reflectionFilter, const vec& refractionFilter) 
		: Material(hasReflection, hasRefraction, refractionIndex, reflectionFilter, refractionFilter) {}

	/// returns the color at texture coordinates
	virtual vec getColor(const vec& textureCoordinates) = 0;
	virtual qvec getColor(const qvec& textureCoordinates) = 0;

	virtual void shade(vec& destination, const vec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray, int index)
	{
		static vec black(0.0f);

		Triangle* triangle = ray.hit[index];
		
		if (triangle) 
		{
			vec rayOrigin(ray.origin.x[index], ray.origin.y[index], ray.origin.z[index]);
			vec rayDirection(ray.dir.x[index], ray.dir.y[index], ray.dir.z[index]);

			vec toEye(-rayDirection);
			normalize(toEye);

			vec normal(triangle->getNormal(ray.u[index], ray.v[index]));

			vec position(rayOrigin + rayDirection * ray.t[index]);
			
			float c = dot(normal, toEye);

			vec tc = triangle->getTexCoord(ray.u[index], ray.v[index]);
			vec color =	getColor(tc);
			Material::endShade(destination, c > 0.0f ? (c * color) : black, color, contribution, level, position, normal, toEye);
		}
	}

	virtual void shade(const quad<vec*>& destination, const qvec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray)
	{
		assert(ray.hit[0] == ray.hit[1] && ray.hit[0] == ray.hit[2] && ray.hit[0] == ray.hit[3]);

		static qvec black(qfloat(0.0f));

		Triangle* triangle = ray.hit[0];
		
		if (triangle) 
		{
			qvec toEye(-ray.dir);
			normalize(toEye);

			qvec normal(triangle->getNormal(ray.u, ray.v));

			qvec position(ray.origin + ray.dir * ray.t);
			
			qfloat c = dot(normal, toEye);
			c[0] = MAX(c[0], 0);
			c[1] = MAX(c[1], 0);
			c[2] = MAX(c[2], 0);
			c[3] = MAX(c[3], 0);

			qvec tc = triangle->getTexCoord(ray.u, ray.v);
			qvec color =	getColor(tc);
			Material::endShade(destination, color * c, color, contribution, level, position, normal, toEye);
		}
	}
};

#endif
