//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _IOPENGLIMAGE_H_
#define _IOPENGLIMAGE_H_

#include "vecmath.h"

class IOpenGLImage
{
public:
	virtual void beginWrite() = 0;
	virtual void setPixel(int x, int y, vec color) = 0;
	virtual void endWrite() = 0;
	virtual void drawFullscreen() = 0;
	virtual void saveToFile(const char* filename) = 0;
	virtual vec* operator[](int y) = 0;
};

#endif
