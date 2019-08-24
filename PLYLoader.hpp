#ifndef PLYLOADER_HPP
#define PLYLOADER_HPP

#include <string>
#include <vector>

#include "Triangle.hpp"
#include "vecmath.h"


#ifndef _int3_def
#define _int3_def
struct int3{
  int data[3];
  int3(int i1, int i2, int i3) {data[0] = i1; data[1] = i2; data[2] = i3;}
	int3() {}
};
#endif

void loadPly(const char *fileName, std::vector<vec>& vertices, std::vector<vec>& normals, std::vector<int3>& faces);
unsigned long getFaceCountFromPly(const char *fileName);

#endif
