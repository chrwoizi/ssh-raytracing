#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "vecmath.h"

#include <string>
#include <iostream>

// windows
#ifndef NULL
#define NULL 0
#endif

class Image
{
public:
	Image()
		: resX(0),resY(0),pixel(NULL)
	{}

	Image(int resX,int resY)
		: resX(resX),resY(resY)
	{
		assert (resX > 0 && resY > 0);
		pixel = new vec[resX*resY];
	}

	~Image()
	{
		if(pixel) delete [] pixel;
	}

	vec *operator[](int y)
	{
		assert (pixel != NULL && 0 <= y && y < resY);
		return pixel+y*resX; 
	}

	vec getPixel(float u, float v)
	{
		assert(u >= 0 && u <= 1);
		if(!(v >= 0 && v <= 1)) std::cout << v << std::endl;
		assert(v >= 0 && v <= 1);
		vec result = pixel[int(v*(resY-1))*resX + int(u*(resX-1))];
		return result;
	}

	qvec getPixel(const qfloat& u, const qfloat& v)
	{
		assert(u[0] >= 0 && u[0] <= 1);
		assert(u[1] >= 0 && u[1] <= 1);
		assert(u[2] >= 0 && u[2] <= 1);
		assert(u[3] >= 0 && u[3] <= 1);
		assert(v[0] >= 0 && v[0] <= 1);
		assert(v[1] >= 0 && v[1] <= 1);
		assert(v[2] >= 0 && v[2] <= 1);
		assert(v[3] >= 0 && v[3] <= 1);

		vec result0 = pixel[int(v[0]*(resY-1))*resX + int(u[0]*(resX-1))];
		vec result1 = pixel[int(v[1]*(resY-1))*resX + int(u[1]*(resX-1))];
		vec result2 = pixel[int(v[2]*(resY-1))*resX + int(u[2]*(resX-1))];
		vec result3 = pixel[int(v[3]*(resY-1))*resX + int(u[3]*(resX-1))];

		qvec result;
		result.x[0] = result0.x;
		result.x[1] = result1.x;
		result.x[2] = result2.x;
		result.x[3] = result3.x;
		result.y[0] = result0.y;
		result.y[1] = result1.y;
		result.y[2] = result2.y;
		result.y[3] = result3.y;
		result.z[0] = result0.z;
		result.z[1] = result1.z;
		result.z[2] = result2.z;
		result.z[3] = result3.z;

		return result;
	}

	void read(const std::string& fileName);
	void readPPM(const std::string& fileName);
	void writePPM(const std::string& fileName, bool clamp = true);

	const std::string& getFileName() { return filename; }

protected:
	std::string filename;
	int resX, resY;
	vec *pixel;
};

#endif
