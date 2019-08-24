//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _TESTSETUP_H_
#define _TESTSETUP_H_

#include <iostream>
#include <fstream>

#include "SimpleScene.hpp"

#include "TestConfig.hpp"

struct TestSetup
{
	double constructionTime;

	unsigned long faceCount;

	bool orderedTraversal;
	bool iterativeTraversal;
	int threads;

	TestSetup()
	{
		clear();
		#ifdef MULTITHREADING
			threads = THREAD_COUNT;
		#else
			threads = 1;
		#endif
		#ifdef TRAVERSE_ORDERED
			orderedTraversal = true;
		#else
			orderedTraversal = false;
		#endif
		#ifdef TRAVERSE_ITERATIVE
			iterativeTraversal = true;
		#else
			iterativeTraversal = false;
		#endif
	}

	void clear()
	{
		constructionTime = -1.0;
	}

	void print(bool testMode, SceneConstructionDetails& construction, unsigned long computedMemoryUsage, const char* modelFile, SCENE_TYPE method, const char* methodStr, unsigned long framesPerTest, Scene* scene, int width, int height)
	{
		print(std::cout, construction, computedMemoryUsage, modelFile, method, methodStr, framesPerTest, scene, width, height);

		if(testMode)
		{
			std::ofstream file;
			file.open("testresults/details.txt", ios_base::app);
			print(file, construction, computedMemoryUsage, modelFile, method, methodStr, framesPerTest, scene, width, height);
			file.close();
		}
	}

	void print(std::ostream& stream, SceneConstructionDetails& construction, unsigned long computedMemoryUsage, const char* modelFile, SCENE_TYPE method, const char* methodStr, unsigned long framesPerTest, Scene* scene, int width, int height)
	{
		stream << "\n--- Test ---\n"
			<< "------------\n"
			<< "acceleration method: " << methodStr << "\n"
			<< "model file: " << modelFile << "\n"
			<< "number of triangles: " << faceCount << "\n"
			<< "scene extends: min(" << scene->getBounds().min.x << "," << scene->getBounds().min.y << "," << scene->getBounds().min.z << ") max(" << scene->getBounds().max.x << "," << scene->getBounds().max.y << "," << scene->getBounds().max.z << ")\n"
			<< "resolution: " << width << "x" << height << "\n"
			<< "threads: " << threads << "\n";
		if(framesPerTest != 0)
		{
			stream << "number of frames: " << framesPerTest << "\n";
		}
		stream << "construction time: " << constructionTime << "\n"
			<< "tree height: " << construction.height << "\n"
			<< "inner node count: " << construction.innerNodes << "\n"
			<< "leaf node count: " << construction.leafNodes << "\n"
			<< "computed memory usage of nodes: " << computedMemoryUsage << "\n";
		if(method == SSH)
		{
			stream << "average node surface ratio approx/real: "
				#ifdef BIGFLOAT_SURFACE_COMPUTATION
					<< construction.bshSurfaceRatio.getdouble()
				#else
					<< construction.bshSurfaceRatio
				#endif
				<< "\n";
		}
		if(method == SSH || method == BVH)
		{
			stream << "traversal algorithm: " << (iterativeTraversal?"iterative":"recursive") << (orderedTraversal?", ordered":"") << "\n";
		}
	}
};


#endif

