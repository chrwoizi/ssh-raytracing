//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _TESTRESULT_H_
#define _TESTRESULT_H_


#include <iostream>
#include <fstream>


struct TestResult
{
	double firstTraversalTime;
	double lastTraversalTime;
	double minTraversalTime;
	double avgTraversalTime;
	double maxTraversalTime;
	double firstRayTraceTime;
	double lastRayTraceTime;
	double minRayTraceTime;
	double avgRayTraceTime;
	double maxRayTraceTime;

	unsigned long rayNodeIntersections;
	unsigned long rayTriangleIntersections;

	TestResult()
	{
		clear();
	}

	void clear()
	{
		firstTraversalTime = -1.0;
		lastTraversalTime = -1.0;
		minTraversalTime = 10000000.0;
		avgTraversalTime = -1.0;
		maxTraversalTime = -1.0;
		firstRayTraceTime = -1.0;
		lastRayTraceTime = -1.0;
		minRayTraceTime = 10000000.0;
		avgRayTraceTime = -1.0;
		maxRayTraceTime = -1.0;
		rayNodeIntersections = 0;
		rayTriangleIntersections = 0;
	}

	void print(int resolutionX, int resolutionY)
	{
		print(std::cout, resolutionX, resolutionY);

		std::ofstream file;
		file.open("testresults/details.txt", ios_base::app);
		print(file, resolutionX, resolutionY);
		file.close();
	}

	void print(std::ostream& stream, int resolutionX, int resolutionY)
	{
		stream << "-----------------------------\n";
		if(rayNodeIntersections != 0)
		{
			stream << "average node intersections per ray: " << 4. * double(rayNodeIntersections) / double(resolutionX*resolutionY) << "\n";
		}
		else
		{
			stream << "average node intersections per ray: not compatible whith multithreading\n";
		}
		if(rayTriangleIntersections != 0)
		{
			stream << "average triangle intersections per ray: " << 4. * double(rayTriangleIntersections) / double(resolutionX*resolutionY) << "\n";
		}
		else
		{
			stream << "average triangle intersections per ray: not compatible whith multithreading\n";
		}
		if(firstTraversalTime > 0)
		{
			stream << "first traversal time: " << firstTraversalTime << "\n"
					<< "last traversal time: " << lastTraversalTime << "\n"
					<< "minimum traversal time: " << minTraversalTime << "\n"
					<< "average traversal time: " << avgTraversalTime << "\n"
					<< "maximum traversal time: " << maxTraversalTime << "\n";
		}
		else
		{
			stream << "first: not compatible whith multithreading.\n";
			stream << "last traversal time: not compatible whith multithreading.\n";
			stream << "minimum traversal time: not compatible whith multithreading.\n";
			stream << "average traversal time: not compatible whith multithreading.\n";
			stream << "maximum traversal time: not compatible whith multithreading.\n";
		}
		stream << "first raytrace time: " << firstRayTraceTime << "\n"
				<< "last raytrace time: " << lastRayTraceTime << "\n"
				<< "minimum raytrace time: " << minRayTraceTime << "\n"
				<< "average raytrace time: " << avgRayTraceTime << "\n"
				<< "maximum raytrace time: " << maxRayTraceTime << "\n\n";
	}

	void printFirstFrame()
	{
		std::cout <<"-----------------------------\n";
		if(firstTraversalTime > 0)
		{
			std::cout << "first traversal time: " << firstTraversalTime << "\n";
		}
		else
		{
			std::cout << "first traversal time: not compatible whith multithreading.\n";
		}
		std::cout << "first raytrace time: " << firstRayTraceTime << "\n";
	}

	void printEveryFrame(int resolutionX, int resolutionY)
	{
		std::cout << "-----------------------------\n";
		if(rayNodeIntersections != 0)
		{
			std::cout << "average node intersections per ray: " << double(rayNodeIntersections) / double(resolutionX*resolutionY) << "\n";
		}
		else
		{
			std::cout << "average node intersections per ray: not compatible whith multithreading\n";
		}
		if(rayTriangleIntersections != 0)
		{
			std::cout << "average triangle intersections per ray: " << double(rayTriangleIntersections) / double(resolutionX*resolutionY) << "\n";
		}
		else
		{
			std::cout << "average triangle intersections per ray: not compatible whith multithreading\n";
		}
		if(firstTraversalTime > 0)
		{
			std::cout << "first traversal time: " << firstTraversalTime << "\n"
					<< "last traversal time: " << lastTraversalTime << "\n"
					<< "minimum traversal time: " << minTraversalTime << "\n"
					<< "average traversal time: " << avgTraversalTime << "\n"
					<< "maximum traversal time: " << maxTraversalTime << "\n";
		}
		else
		{
			std::cout << "first: not compatible whith multithreading.\n";
			std::cout << "last traversal time: not compatible whith multithreading.\n";
			std::cout << "minimum traversal time: not compatible whith multithreading.\n";
			std::cout << "average traversal time: not compatible whith multithreading.\n";
			std::cout << "maximum traversal time: not compatible whith multithreading.\n";
		}
		std::cout << "last raytrace time: " << lastRayTraceTime << "\n"
			<< "minimum raytrace time: " << minRayTraceTime << "\n"
			<< "average raytrace time: " << avgRayTraceTime << "\n"
			<< "maximum raytrace time: " << maxRayTraceTime << "\n";
	}
};


#endif

