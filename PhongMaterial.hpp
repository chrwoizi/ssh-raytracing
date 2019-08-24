#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H

#include "Material.hpp"

#define BLINN

class PhongMaterial : public Material
{
public:
	/// <refraction> index represents the density.
	/// <reflection> is the amount of blending between reflection and phong shading result
	/// <refractionFilter> is multiplied with refraction raytrace result. it represents the object inside's color.
	PhongMaterial(bool hasReflection, bool hasRefraction, float refractionIndex, const vec& reflectionFilter, const vec& refractionFilter)
		: Material(hasReflection, hasRefraction, refractionIndex, reflectionFilter, refractionFilter) {}

	/// returns the ambient color at texture coordinates
	virtual vec getAmbientColor(const vec& textureCoordinates) = 0;
	virtual qvec getAmbientColor(const qvec& textureCoordinates) = 0;

	/// returns the diffuse color at texture coordinates
	virtual vec getDiffuseColor(const vec& textureCoordinates) = 0;
	virtual qvec getDiffuseColor(const qvec& textureCoordinates) = 0;

	/// returns the specular color at texture coordinates
	virtual vec getSpecularColor(const vec& textureCoordinates) = 0;
	virtual qvec getSpecularColor(const qvec& textureCoordinates) = 0;

	/// returns the shininess exponent at texture coordinates
	virtual float getShininess(const vec& textureCoordinates) = 0;
	virtual qfloat getShininess(const qvec& textureCoordinates) = 0;
	
	virtual void shade(vec& destination, const vec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray, int index)
	{
		static vec ambient = vec(0.05f);
		
		Triangle* triangle = ray.hit[index];
		
		if(!triangle) return;

		// texture coordinates
		vec tc = triangle->getTexCoord(ray.u[index], ray.v[index]);
		
		// material diffuse color
		vec diffuseColor = getDiffuseColor(tc);
		vec specularColor = getSpecularColor(tc);
		float shininess = getShininess(tc);
			
		vec rayDirection(ray.dir.x[index], ray.dir.y[index], ray.dir.z[index]);
		vec rayOrigin(ray.origin.x[index], ray.origin.y[index], ray.origin.z[index]);

		// hit position
		vec position = rayOrigin + rayDirection*ray.t[index];

		// vector from position to camera
		vec toEye = -rayDirection;
		normalize(toEye);

		// surface normal at uv
		vec normal = triangle->getNormal(ray.u[index], ray.v[index]);

		vec sum = ambient * getAmbientColor(tc);
		for(unsigned int i = 0; i < lights.size(); ++i)
		{
			Light* light = lights[i];

			int sampleCount = light->getSampleCount();
			float invSampleCount = 1.f / float(sampleCount);

			for(int sample = 0; sample < sampleCount; ++sample)
			{
				vec samplePosition = light->getSamplePosition(sample);

				// direction from position to light source
				vec toLight = light->getDirection(position, samplePosition);

				vec lightColor = light->getColor(position, samplePosition) * invSampleCount;
				
				// diffuse contribution
				float d = dot(normal, toLight);
				d = MAX(d, 0);

				vec diffuseLightColor = lightColor * d;
				
				// specular contribution
				#ifdef BLINN
					vec halfway = toLight + toEye;
					normalize(halfway);
					d = dot(halfway, normal);
				#else
					vec reflected = normal*(2.0f*dot(toLight, normal)) - toLight;
					d = dot(reflected, toEye);
				#endif

				d = MAX(d, 0);
				
				vec specularLightColor = lightColor * powf(d, shininess);

				vec result = diffuseLightColor*diffuseColor + specularLightColor*specularColor;

				if(receiveShadows)
				{
					vec resultc = result*contribution;
					if(resultc.x > 1e-5f || resultc.y > 1e-5f || resultc.z > 1e-5f)
					{
						#ifdef ITERATIVE_SHADOWS
							RayTracer::getInstance().createShadowRay(position, toLight, light->getDistance(position, samplePosition), resultc, quad<vec*>(&destination,0,0,0));
						#else
							RayTracer::getInstance().castShadowRay(position, toLight, light->getDistance(position, samplePosition), resultc, quad<vec*>(&destination,0,0,0));
						#endif
					}
				}
				else
				{
					sum += result;
				}
			}
		}

		if(receiveShadows)
		{
			destination += sum;
		}
		else
		{
			Material::endShade(destination, sum, specularColor, contribution, level, position, normal, toEye);
		}
	}

	virtual void shade(const quad<vec*>& destination, const qvec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray)
	{
		static qvec ambient(qfloat(0.05f));
		
		Triangle* triangle = ray.hit[0];
		
		if(!triangle) return;

		// texture coordinates
		qvec tc = triangle->getTexCoord(ray.u, ray.v);
		
		// material diffuse color
		qvec diffuseColor = getDiffuseColor(tc);
		qvec specularColor = getSpecularColor(tc);
		qfloat shininess = getShininess(tc);

		// hit position
		qvec position = ray.origin + ray.dir*ray.t;

		// surface normal at uv
		qvec normal = triangle->getNormal(ray.u, ray.v);
			
		// vector from position to camera
		qvec toEye = -ray.dir;
		normalize(toEye);

		qvec sum = ambient * getAmbientColor(tc);
		for(unsigned int i = 0; i < lights.size(); ++i)
		{
			Light* light = lights[i];

			int sampleCount = light->getSampleCount();
			float invSampleCount = 1.f / float(sampleCount);

			for(int sample = 0; sample < sampleCount; ++sample)
			{
				qvec samplePosition = light->getSamplePosition(sample);

				// direction from position to light source
				qvec toLight = light->getDirection(position, samplePosition);

				qvec lightColor = light->getColor(position, samplePosition) * invSampleCount;
				
				// diffuse contribution
				qfloat d = dot(normal, toLight);
				d[0] = MAX(d[0], 0);
				d[1] = MAX(d[1], 0);
				d[2] = MAX(d[2], 0);
				d[3] = MAX(d[3], 0);

				qvec diffuseLightColor = lightColor * d;
				
				// specular contribution
				#ifdef BLINN
					qvec halfway = toLight + toEye;
					normalize(halfway);
					d = dot(halfway, normal);
				#else
					qvec reflected = normal*(qfloat(2.0f)*dot(toLight, normal)) - toLight;
					d = dot(reflected, toEye);
				#endif

				d[0] = MAX(d[0], 0);
				d[1] = MAX(d[1], 0);
				d[2] = MAX(d[2], 0);
				d[3] = MAX(d[3], 0);
				
				qfloat _pow;
				_pow[0] = powf(d[0], shininess[0]);
				_pow[1] = powf(d[1], shininess[1]);
				_pow[2] = powf(d[2], shininess[2]);
				_pow[3] = powf(d[3], shininess[3]);
				qvec specularLightColor = lightColor * _pow;

				qvec result = diffuseLightColor*diffuseColor + specularLightColor*specularColor;
				
				if(receiveShadows)
				{
					qvec resultc = result*contribution;
					if(((resultc.x > 1e-5f) | (resultc.y > 1e-5f) | (resultc.z > 1e-5f)).mask())
					{
						#ifdef ITERATIVE_SHADOWS
							RayTracer::getInstance().createShadowRay(position, toLight, light->getDistance(position, samplePosition), resultc, destination);
						#else
							RayTracer::getInstance().castShadowRay(position, toLight, light->getDistance(position, samplePosition), resultc, destination);
						#endif
					}
				}
				else
				{
					sum += result;
				}
			}
		}

		if(receiveShadows)
		{
			*destination[0] += vec(sum.x[0], sum.y[0], sum.z[0]);
			*destination[1] += vec(sum.x[1], sum.y[1], sum.z[1]);
			*destination[2] += vec(sum.x[2], sum.y[2], sum.z[2]);
			*destination[3] += vec(sum.x[3], sum.y[3], sum.z[3]);
		}
		else
		{
			Material::endShade(destination, sum, specularColor, contribution, level, position, normal, toEye);
		}
	}
};

#endif
