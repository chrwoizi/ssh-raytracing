//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _OPENGLTEXTURE_H_
#define _OPENGLTEXTURE_H_

#include "IOpenGLImage.hpp"
#include "Image.hpp"

#include <GL/glut.h>

class OpenGLTexture : public IOpenGLImage
{
private:
	GLuint textureId;	
	unsigned int width;
	unsigned int height;
	Image img;

public:
	OpenGLTexture(unsigned int width, unsigned int height);

	virtual void setPixel(int x, int y, vec color);
	virtual vec* operator[](int y);
	
	virtual void beginWrite();
	virtual void endWrite();

	virtual void drawFullscreen();
	virtual void saveToFile(const char* filename);
};

#endif