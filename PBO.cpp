//
// Description: 
//
//
// Author: Christian Woizischke
//
//

#include <GL/glew.h>

#include <iostream>
#include <string.h>

#include "PBO.hpp"

using namespace std;

PBO::PBO(unsigned int width, unsigned int height, bool textureQuad)
{
	mapped = 0;
	
	this->width = width;
	this->height = height;
	this->textureQuad = textureQuad;
	
	// device must support PBO to create an instance
	if(!supportedByDevice()) throw "Check for device capabilities before instance creation!";

	if(textureQuad)
	{
		// create texture
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// create pixel buffer object
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferId);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 3*width*height*sizeof(float), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void PBO::beginWrite()
{
	// bind and map buffer (enables writing)
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferId);
	mapped = reinterpret_cast<vec*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
}

void PBO::endWrite()
{
	// unmap buffer (end of write)
	if (!glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER))
	{
		cerr << "Couldn't unmap pixel buffer. Exiting\n";
		assert(false);
	}

	if(textureQuad)
	{
		// copy from buffer to texture
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void PBO::setPixel(int x, int y, vec color)
{
	memcpy(&mapped[y*width + x], &color, sizeof(vec));
}

vec* PBO::operator[](int y)
{
	return &mapped[y*width];
}

void PBO::drawFullscreen()
{
	if(textureQuad)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 100.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 100.0f, 1.0f);
		glEnd();
	}
	else
	{
		glDrawPixels(width, height, GL_RGB, GL_FLOAT, 0);
	}
}

bool PBO::supportedByDevice()
{
	if(!glewGetExtension("GL_EXT_pixel_buffer_object"))
	{
		cout << "GL_EXT_pixel_buffer_object not supported by device." << endl;
		
		return false;
	}

	return true;
}

void PBO::saveToFile(const char* filename)
{
	// not implemented
}