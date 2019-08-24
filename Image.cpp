#include <fstream>
#include <iostream>
#include <string.h>

#include "Image.hpp"
#include "CImg.h"


void eatComments(FILE *f)
{
	int ch;
	
	while((ch=getc(f))=='#') {
		char str[1000];
		fgets(str,1000,f);
	}
	ungetc(ch,f);
}


void eatWhitespace(FILE *f)
{
	int ch=getc(f);
	
	while(ch==' ' || ch=='\t' || ch=='\n' || ch=='\f' || ch=='\r')
		ch=getc(f);
	
	ungetc(ch,f);
}

void Image::read(const std::string& fileName)
{
	std::cout << "reading image " << fileName << std::endl;

	this->filename = fileName;
	
	cimg_library::CImg<float> img;
	try
	{
		img.load(fileName.c_str());
		
		resX = img.dimx();
		resY = img.dimy();
		assert(img.dimv() == 3);

		if(pixel) delete[] pixel;
		pixel = new vec[resX*resY];

		for(int y = 0; y < resY; ++y)
		{
			for(int x = 0; x < resX; ++x)
			{
				pixel[y*resX + x] = vec(img.at(x,resY-y-1,0,0), img.at(x,resY-y-1,0,1), img.at(x,resY-y-1,0,2)) * (1.f/256.f);
			}
		}
	}
	catch(...)
	{
		resX = 1;
		resY = 1;
		if(pixel) delete[] pixel;
		pixel = new vec[resX*resY];
		pixel[0] = vec(1,0,1);
	}
}

void Image::readPPM(const std::string& fileName)
{
	std::cout << "reading PPM image " << fileName << std::endl;

	this->filename = fileName;
	
	FILE *f;
	char ch;
	int width, height, colres;
	
	f = fopen(fileName.c_str(),"r");
	if (f == NULL) {
		std::cerr << "could not open file " << fileName << std::endl;
		exit(1);
	}
	
	char str[1000];
	
	eatWhitespace(f);
	eatComments(f);
	eatWhitespace(f);
	fscanf(f,"%s",str);
	
	if (!strcmp(str,"P3")) {
		eatWhitespace(f);
		eatComments(f);
		eatWhitespace(f);
		
		fscanf(f,"%d %d",&width,&height);
		if(width<=0 || height<=0) {
			std::cerr << "width and height of the image are not greater than zero in file " << fileName << std::endl;
			exit(1);
		}

#ifndef NDEBUG
		std::cout << "Image Res: " << width << " " << height << std::endl;
#endif
		
		resX = width;
		resY = height;
		
		delete [] pixel;
		pixel = new vec[resX*resY];
		
		eatWhitespace(f);
		eatComments(f);
		eatWhitespace(f);
		fscanf(f,"%d",&colres);
		
		ch=0;
		while(ch!='\n')
			fscanf(f,"%c",&ch);
		
		for (int y=resY-1;y>=0;y--)
			for (int x=0;x<resX;x++) {
				int c[3];
				fscanf(f,"%d %d %d",c+0,c+1,c+2);
				(*this)[y][x] = vec(c[0] / float(colres),
					c[1] / float(colres),
					c[2] / float(colres));
			}
		
		fclose(f);
	} else {
		std::cerr << "wrong format of file " << fileName<< std::endl;
		exit(1);
	}
}


void Image::writePPM(const std::string& fileName, bool clamp)
{
	std::ofstream file(fileName.c_str());
	file << "P3" << std::endl;
	file << resX << " " << resY << " " << 255 << std::endl;
	for (int y=resY-1;y>=0;y--) {
		for (int x=0;x<resX;x++) {
			vec rgb((*this)[y][x]);
			if (clamp) {
				for (int c = 0; c < 3; ++c)
					rgb[c] = rgb[c] > 1.0f ? 1.0f : (rgb[c] < 0.0f ? 0.0f : rgb[c]);
			}
			file 
			<< (int)(255.99999999 * rgb.x) << " "
			<< (int)(255.99999999 * rgb.y) << " "
			<< (int)(255.99999999 * rgb.z) << " "
			<< "\t";
		}
    	file << std::endl;
    	file << std::flush;
    }
}

