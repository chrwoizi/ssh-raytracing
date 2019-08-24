#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vecmath.h"
#include "Ray.hpp"
#include "AABBox.hpp"
#include "Material.hpp"
#include <iostream>

class Triangle
{
  public:
	Triangle(Material* material, vec a, vec b, vec c, vec na, vec nb, vec nc, vec ta, vec tb, vec tc)
	: material(material), a(a),edge_ab(b-a),edge_ac(c-a), na(na),nb(nb),nc(nc),ta(ta),tb(tb),tc(tc)
	{		
		normalize(na);
		normalize(nb);
		normalize(nc);

		if(ta.x != 1.0f) ta.x = fmod(ta.x, 1.0f);
		if(ta.x < 0.0f) ta.x = 1.0f + ta.x;
		assert(ta.x >= 0 && ta.x <= 1);
		
		if(ta.y != 1.0f) ta.y = fmod(ta.y, 1.0f);
		if(ta.y < 0.0f) ta.y = 1.0f + ta.y;
		assert(ta.y >= 0 && ta.y <= 1);

		if(tb.x != 1.0f) tb.x = fmod(tb.x, 1.0f);
		if(tb.x < 0.0f) tb.x = 1.0f + tb.x;
		assert(tb.x >= 0 && tb.x <= 1);
		
		if(tb.y != 1.0f) tb.y = fmod(tb.y, 1.0f);
		if(tb.y < 0.0f) tb.y = 1.0f + tb.y;
		assert(tb.y >= 0 && tb.y <= 1);

		if(tc.x != 1.0f) tc.x = fmod(tc.x, 1.0f);
		if(tc.x < 0.0f) tc.x = 1.0f + tc.x;
		assert(tc.x >= 0 && tc.x <= 1);
		
		if(tc.y != 1.0f) tc.y = fmod(tc.y, 1.0f);
		if(tc.y < 0.0f) tc.y = 1.0f + tc.y;
		assert(tc.y >= 0 && tc.y <= 1);
	}
	
	void intersect(PackedRay&);
	
	const AABBox getBounds() const 
	{ 
		AABBox bounds;
		bounds.extend(a);
		bounds.extend(a + edge_ab);
		bounds.extend(a + edge_ac);
		return bounds; 
	}
	
	vec getNormal(float u, float v)
	{
		vec normal = nb*u + nc*v + na*(1.0f - u - v);
		normalize(normal);
		return normal;
	}
	
	qvec getNormal(const qfloat& u, const qfloat& v)
	{
		qvec normal = qvec(nb)*u + qvec(nc)*v + qvec(na)*(qfloat(1.0f) - u - v);
		normalize(normal);
		return normal;
	}
	
	vec getTexCoord(float u, float v)
	{
		if(ta.x != 1.0f) ta.x = fmod(ta.x, 1.0f);
		if(ta.x < 0.0f) ta.x = 1.0f + ta.x;
		assert(ta.x >= 0 && ta.x <= 1);
		
		if(ta.y != 1.0f) ta.y = fmod(ta.y, 1.0f);
		if(ta.y < 0.0f) ta.y = 1.0f + ta.y;
		assert(ta.y >= 0 && ta.y <= 1);

		if(tb.x != 1.0f) tb.x = fmod(tb.x, 1.0f);
		if(tb.x < 0.0f) tb.x = 1.0f + tb.x;
		assert(tb.x >= 0 && tb.x <= 1);
		
		if(tb.y != 1.0f) tb.y = fmod(tb.y, 1.0f);
		if(tb.y < 0.0f) tb.y = 1.0f + tb.y;
		assert(tb.y >= 0 && tb.y <= 1);

		if(tc.x != 1.0f) tc.x = fmod(tc.x, 1.0f);
		if(tc.x < 0.0f) tc.x = 1.0f + tc.x;
		assert(tc.x >= 0 && tc.x <= 1);
		
		if(tc.y != 1.0f) tc.y = fmod(tc.y, 1.0f);
		if(tc.y < 0.0f) tc.y = 1.0f + tc.y;
		assert(tc.y >= 0 && tc.y <= 1);
		
		/*std::cout << ta.x << " " << ta.y << " " << tb.x << " " << tb.y << " " << tc.x << " " << tc.y << " " << std::endl;
		
		if(!(u >= 0 && u <= 1)) std::cout << u << std::endl;
		assert(u >= 0 && u <= 1);
		if(!(v >= 0 && v <= 1)) std::cout << v << std::endl;
		assert(v >= 0 && v <= 1);*/
		
		vec texcoord = tb*u + tc*v + ta*(1.0f - u - v);

		/*if(!(texcoord.x >= 0 && texcoord.x <= 1)) std::cout << texcoord.x << std::endl;
		assert(texcoord.x >= 0 && texcoord.x <= 1);
		if(!(texcoord.y >= 0 && texcoord.y <= 1)) std::cout << texcoord.y << std::endl;
		assert(texcoord.y >= 0 && texcoord.y <= 1);*/
		
		return texcoord;
	}
	
	qvec getTexCoord(const qfloat& u, const qfloat& v)
	{
		if(ta.x != 1.0f) ta.x = fmod(ta.x, 1.0f);
		if(ta.x < 0.0f) ta.x = 1.0f + ta.x;
		assert(ta.x >= 0 && ta.x <= 1);
		
		if(ta.y != 1.0f) ta.y = fmod(ta.y, 1.0f);
		if(ta.y < 0.0f) ta.y = 1.0f + ta.y;
		assert(ta.y >= 0 && ta.y <= 1);

		if(tb.x != 1.0f) tb.x = fmod(tb.x, 1.0f);
		if(tb.x < 0.0f) tb.x = 1.0f + tb.x;
		assert(tb.x >= 0 && tb.x <= 1);
		
		if(tb.y != 1.0f) tb.y = fmod(tb.y, 1.0f);
		if(tb.y < 0.0f) tb.y = 1.0f + tb.y;
		assert(tb.y >= 0 && tb.y <= 1);

		if(tc.x != 1.0f) tc.x = fmod(tc.x, 1.0f);
		if(tc.x < 0.0f) tc.x = 1.0f + tc.x;
		assert(tc.x >= 0 && tc.x <= 1);
		
		if(tc.y != 1.0f) tc.y = fmod(tc.y, 1.0f);
		if(tc.y < 0.0f) tc.y = 1.0f + tc.y;
		assert(tc.y >= 0 && tc.y <= 1);
		
		/*std::cout << a.x << " " << a.y << " " << edge_ab.x << " " << edge_ab.y << " " << edge_ac.x << " " << edge_ac.y << " " << std::endl;
		std::cout << ta.x << " " << ta.y << " " << tb.x << " " << tb.y << " " << tc.x << " " << tc.y << " " << std::endl;
		
		if(!(u[0] >= 0 && u[0] <= 1)) std::cout << u[0] << std::endl;
		assert(u[0] >= 0 && u[0] <= 1);
		if(!(v[0] >= 0 && v[0] <= 1)) std::cout << v[0] << std::endl;
		assert(v[0] >= 0 && v[0] <= 1);*/
		
		qvec texcoord = qvec(tb)*u + qvec(tc)*v + qvec(ta)*(qfloat(1.0f) - u - v);

		/*if(!(texcoord.x[0] >= 0 && texcoord.x[0] <= 1))
		{
			std::cout << u[0] << " " << v[0] << " " << texcoord.x[0] << std::endl;
			std::cout << ta.x << " " << ta.y << " " << tb.x << " " << tb.y << " " << tc.x << " " << tc.y << " " << std::endl;
		}
		assert(texcoord.x[0] >= 0 && texcoord.x[0] <= 1);
		if(!(texcoord.y[0] >= 0 && texcoord.y[0] <= 1)) std::cout << texcoord.y[0] << std::endl;
		assert(texcoord.y[0] >= 0 && texcoord.y[0] <= 1);*/
		
		return texcoord;
	}

	Material* material;
	
	static unsigned intersectionTestsPerformed;
	
  private:
	vec a, edge_ab, edge_ac, na, nb, nc, ta, tb, tc;
};
#endif
