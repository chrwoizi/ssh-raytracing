//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _SCENECONSTRUCTIONDETAILS_H_
#define _SCENECONSTRUCTIONDETAILS_H_

#include "bigfloat.h"

#include "TestConfig.hpp"

struct SceneConstructionDetails
{
	unsigned long innerNodes;
	unsigned long leafNodes;
	unsigned int height;

#ifdef BIGFLOAT_SURFACE_COMPUTATION
	BigFloat bshSurfaceRatio;
#else
	long double bshSurfaceRatio;
#endif

	SceneConstructionDetails() : bshSurfaceRatio(0.0)
	{
		innerNodes = 0;
		leafNodes = 0;
		height = 0;
	}
};

#endif