#ifndef SKYBOXMATERIAL_HPP
#define SKYBOXMATERIAL_HPP

#include "Material.hpp"

#define MAX(a,b) ((a)>(b)?(a):(b))

class SkyboxMaterial : public Material
{
public:
	SkyboxMaterial() : Material(false, false, 0, vec(1,1,1), vec(1,1,1)) {}

	virtual void shade(vec& destination, const vec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray, int index)
	{
		assert(ray.hit[index]);

		vec rayOrigin(ray.origin.x[index], ray.origin.y[index], ray.origin.z[index]);
		vec rayDirection(ray.dir.x[index], ray.dir.y[index], ray.dir.z[index]);

		vec position(rayOrigin + rayDirection * ray.t[index]);

		normalize(position);

		position = position * 0.5f + vec(0.5f);

		vec color = position*contribution;
		destination += color;
	}

	virtual void shade(const quad<vec*>& destination, const qvec& contribution, int level, std::vector<Light*>& lights, PackedRay &ray)
	{
		qvec position(ray.origin + ray.dir * ray.t);

		normalize(position);

		position = position * qfloat(0.5f) + qvec(qfloat(0.5f));

		qvec color = position*contribution;
		*destination[0] += vec(color.x[0], color.y[0], color.z[0]);
		*destination[1] += vec(color.x[1], color.y[1], color.z[1]);
		*destination[2] += vec(color.x[2], color.y[2], color.z[2]);
		*destination[3] += vec(color.x[3], color.y[3], color.z[3]);
	}
};

#endif
