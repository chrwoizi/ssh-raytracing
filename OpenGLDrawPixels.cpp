//
// Description: 
//
//
// Author: Christian Woizischke
//
//

#include "OpenGLDrawPixels.hpp"

OpenGLDrawPixels::OpenGLDrawPixels(unsigned int width, unsigned int height) : img(width, height)
{
	this->width = width;
	this->height = height;
}

void OpenGLDrawPixels::beginWrite()
{
}

void OpenGLDrawPixels::endWrite()
{
}

void OpenGLDrawPixels::setPixel(int x, int y, vec color)
{
	img[y][x] = color;
}

vec* OpenGLDrawPixels::operator[](int y)
{
	return img[y];
}

void OpenGLDrawPixels::drawFullscreen()
{
	glRasterPos2i(0, 0);
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, img[0]);
	//glFlush();
}

void OpenGLDrawPixels::saveToFile(const char* filename)
{
	img.writePPM(filename, true);
}