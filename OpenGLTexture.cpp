//
// Description: 
//
//
// Author: Christian Woizischke
//
//

#include "OpenGLTexture.hpp"

OpenGLTexture::OpenGLTexture(unsigned int width, unsigned int height) : img(width, height)
{
	this->width = width;
	this->height = height;
	glGenTextures(1, &textureId);
	
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLTexture::beginWrite()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void OpenGLTexture::endWrite()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, img[0]);
}

void OpenGLTexture::setPixel(int x, int y, vec color)
{
	img[y][x] = color;
}

vec* OpenGLTexture::operator[](int y)
{
	return img[y];
}

void OpenGLTexture::drawFullscreen()
{
	glBindTexture(GL_TEXTURE_2D, textureId);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 0.0f, 1.0f); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 100.0f, 1.0f); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 100.0f, 1.0f); 
	glEnd();
}

void OpenGLTexture::saveToFile(const char* filename)
{
	img.writePPM(filename, true);
}