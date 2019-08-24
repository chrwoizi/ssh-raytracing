//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _PBO_H_
#define _PBO_H_

#include "IOpenGLImage.hpp"

#include <GL/glut.h>

class PBO : public IOpenGLImage
{
	private:
		GLuint bufferId;
		GLuint textureId;
		unsigned int width;
		unsigned int height;
		vec* mapped;
		bool textureQuad;

	public:
		// param textureQuad
		// 	true: copy to texture and draw fullscreen quad with texture on it.
		// 	false: use glDrawPixels
		PBO(unsigned int width, unsigned int height, bool textureQuad);

		virtual void setPixel(int x, int y, vec color);
		virtual vec* operator[](int y);
	
		virtual void beginWrite();
		virtual void endWrite();

		// draws a fullscreen quad with the buffer as texture
		virtual void drawFullscreen();
		
		virtual void saveToFile(const char* filename);

		// returns true if pbuffer technology is supported by the graphics device
		static bool supportedByDevice();
};

#endif