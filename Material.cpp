
#include "Material.hpp"
#include "RayTracer.hpp"

void Material::endShade(vec& destination, const vec& surfaceColor, const vec& specularColor, const vec& contribution, int level, const vec& position, const vec& normal, const vec& toEye)
{
	if((!hasReflection && !hasRefraction) || level <= 0)
	{
		//maximum  number of reflections/refractions reached
		destination += contribution * surfaceColor;
	}
	else
	{
		if(hasReflection || hasRefraction)
		{
			bool fromInside = dot(normal, toEye) < 0.f;

			float reflection_refraction;

			float density1 = fromInside ? refractionIndex : 1;
			float density2 = fromInside ? 1 : refractionIndex;
			float viewProjection = dot(toEye, normal);
			float cosThetaI = fabsf(viewProjection); 
			float cosThetaT;
			float sinThetaI = sqrtf(1 - cosThetaI * cosThetaI);
			float sinThetaT = (density1 / density2) * sinThetaI;
			if (sinThetaT * sinThetaT > 0.9999f)
			{
				reflection_refraction = 1.0f ; // pure reflectance at grazing angle
				cosThetaT = 0.0f;
			}
			else
			{
				cosThetaT = sqrtf(1 - sinThetaT * sinThetaT);
				float reflectanceOrtho = 
				  (density2 * cosThetaT - density1 * cosThetaI ) 
				  / (density2 * cosThetaT + density1  * cosThetaI);
				reflectanceOrtho = reflectanceOrtho * reflectanceOrtho;
				float reflectanceParal = 
				  (density1 * cosThetaT - density2 * cosThetaI )
				  / (density1 * cosThetaT + density2 * cosThetaI);
				reflectanceParal = reflectanceParal * reflectanceParal;

				reflection_refraction = 0.5f * (reflectanceOrtho + reflectanceParal);
			}

			// surface color (i.e. phong)
			destination += surfaceColor * contribution;

			// reflection
			if(!fromInside && hasReflection && reflection_refraction > 0)
			{
				vec reflected = normal * dot(normal, toEye) * 2.0f - toEye;
				RayTracer::getInstance().createRefxxctionRay(position, reflected, quad<vec*>(&destination,0,0,0), level-1, reflection_refraction * reflectionFilter * contribution);
			}

			// refraction
			if(hasRefraction && reflection_refraction < 1)
			{
				vec refracted = (-toEye + normal*cosThetaI)*(sinThetaT/sinThetaI) - normal*cosThetaT;
				RayTracer::getInstance().createRefxxctionRay(position, refracted, quad<vec*>(&destination,0,0,0), level-1, refractionFilter * (1-reflection_refraction) * contribution);
			}
		}
		else
		{
			// surface color (i.e. phong)
			destination += contribution * surfaceColor;
		}
	}
}

void Material::endShade(const quad<vec*>& destination, const qvec& surfaceColor, const qvec& specularColor, const qvec& contribution, int level, const qvec& position, const qvec& normal, const qvec& toEye)
{
	if((!hasReflection && !hasRefraction) || level <= 0)
	{
		//maximum  number of reflections/refractions reached
		qvec res = contribution * surfaceColor;
		*destination[0] += vec(res.x[0], res.y[0], res.z[0]);
		*destination[1] += vec(res.x[1], res.y[1], res.z[1]);
		*destination[2] += vec(res.x[2], res.y[2], res.z[2]);
		*destination[3] += vec(res.x[3], res.y[3], res.z[3]);
	}
	else
	{
		qfloat reflection_refraction;

		if(hasReflection || hasRefraction)
		{
			// does the ray come from the inside of the model (opposite of normal direction)?
			bool fromInside = dot(vec(normal.x[0], normal.y[0], normal.z[0]), vec(toEye.x[0], toEye.y[0], toEye.z[0])) < 0.f;

			qfloat reflection_refraction;

			qfloat density1 = fromInside ? qrefractionIndex : 1;
			qfloat density2 = fromInside ? 1 : qrefractionIndex;
			qfloat viewProjection = dot(toEye, normal);
			qfloat cosThetaI; 
			cosThetaI[0] = fabsf(viewProjection[0]);
			cosThetaI[1] = fabsf(viewProjection[1]);
			cosThetaI[2] = fabsf(viewProjection[2]);
			cosThetaI[3] = fabsf(viewProjection[3]);
			qfloat cosThetaT;
			qfloat sinThetaISquared = 1 - cosThetaI * cosThetaI;
			qfloat sinThetaI;
			sinThetaI[0] = sqrtf(sinThetaISquared[0]);
			sinThetaI[1] = sqrtf(sinThetaISquared[1]);
			sinThetaI[2] = sqrtf(sinThetaISquared[2]);
			sinThetaI[3] = sqrtf(sinThetaISquared[3]);
			qfloat sinThetaT = (density1 / density2) * sinThetaI;
			if((sinThetaT * sinThetaT > qfloat(0.9999f)).mask())
			{
				cosThetaT = 0.0f;
				reflection_refraction = 1.0f ; // pure reflectance at grazing angle
			}
			else
			{
				qfloat tmp2 = 1 - sinThetaT * sinThetaT;
				cosThetaT[0] = sqrtf(tmp2[0]);
				cosThetaT[1] = sqrtf(tmp2[1]);
				cosThetaT[2] = sqrtf(tmp2[2]);
				cosThetaT[3] = sqrtf(tmp2[3]);
				
				qfloat reflectanceOrtho = 
				  (density2 * cosThetaT - density1 * cosThetaI ) 
				  / (density2 * cosThetaT + density1  * cosThetaI);
				reflectanceOrtho = reflectanceOrtho * reflectanceOrtho;
				qfloat reflectanceParal = 
				  (density1 * cosThetaT - density2 * cosThetaI )
				  / (density1 * cosThetaT + density2 * cosThetaI);
				reflectanceParal = reflectanceParal * reflectanceParal;

				reflection_refraction = qfloat(0.5f) * (reflectanceOrtho + reflectanceParal);
			}

			// surface color (i.e. phong)
			qvec tmp = surfaceColor * contribution;
			*destination[0] += vec(tmp.x[0], tmp.y[0], tmp.z[0]);
			*destination[1] += vec(tmp.x[1], tmp.y[1], tmp.z[1]);
			*destination[2] += vec(tmp.x[2], tmp.y[2], tmp.z[2]);
			*destination[3] += vec(tmp.x[3], tmp.y[3], tmp.z[3]);
	
			// reflection
			if(!fromInside && hasReflection && (reflection_refraction > 0).mask())
			{
				qvec reflected = normal * dot(normal, toEye) * qfloat(2.0f) - toEye;
				RayTracer::getInstance().castRefxxctionRay(position, reflected, destination, level-1, qreflectionFilter * reflection_refraction * contribution);
			}

			// refraction
			if(hasRefraction && (reflection_refraction < 1).mask())
			{
				qvec refracted = (-toEye + normal*cosThetaI)*(sinThetaT/sinThetaI) - normal*cosThetaT;
				RayTracer::getInstance().createRefxxctionRay(position, refracted, destination, level-1, qrefractionFilter * (1-reflection_refraction) * contribution);
			}
		}
		else
		{
			// surface color (i.e. phong)
			qvec tmp = surfaceColor * contribution;
			*destination[0] += vec(tmp.x[0], tmp.y[0], tmp.z[0]);
			*destination[1] += vec(tmp.x[1], tmp.y[1], tmp.z[1]);
			*destination[2] += vec(tmp.x[2], tmp.y[2], tmp.z[2]);
			*destination[3] += vec(tmp.x[3], tmp.y[3], tmp.z[3]);
		}
	}
}
