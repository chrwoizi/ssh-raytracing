//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _TIME_MEASUREMENT_H_
#define _TIME_MEASUREMENT_H_

#include <iostream>
#include <stdlib.h>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

using namespace std;

class TimeMeasurement
{
private:
	#ifdef WINDOWS
		LARGE_INTEGER perfCounterFrequency;
		LARGE_INTEGER start;
	#else
		timeval start;
	#endif
	double accumulator;
	double timeSum;
	int frames;
	bool pausing;
	
public:
	TimeMeasurement()
	{
		pausing = false;
		accumulator = 0;
		timeSum = 0;
		frames = 0;
		#ifdef WINDOWS
			QueryPerformanceFrequency(&perfCounterFrequency);
			QueryPerformanceCounter(&start);
		#else
			gettimeofday(&start, NULL);
		#endif
	}

	void startAverageMeasurement(unsigned short waitNFrames)
	{
		timeSum = 0;
		frames = -waitNFrames;
	}

	void restart()
	{
		pausing = false;
		accumulator = 0;
		#ifdef WINDOWS
			QueryPerformanceCounter(&start);
		#else
			gettimeofday(&start, NULL);
		#endif
	}

	void pause()
	{
		accumulator = getCurrentTime();
		pausing = true;
	}

	void resume()
	{
		// set new start. time between previous start and pause has been saved in accumulator and will be added to upcoming getCurrentTime calls
		#ifdef WINDOWS
			QueryPerformanceCounter(&start);
		#else
			gettimeofday(&start, NULL);
		#endif
		pausing = false;
	}

	double getCurrentTime()
	{
		#ifdef WINDOWS
			LARGE_INTEGER current;
			QueryPerformanceCounter(&current);
		#else
			timeval current;
			gettimeofday(&current, NULL);
		#endif

		if(pausing) return accumulator;
		
		#ifdef WINDOWS
			return accumulator + (double)(current.QuadPart - start.QuadPart) / (double)perfCounterFrequency.QuadPart;
		#else
			return accumulator + double(current.tv_sec - start.tv_sec) + double(current.tv_usec - start.tv_usec)/1000000.0;
		#endif
	}

	void printCurrentTime(const char* text)
	{
		double time = getCurrentTime();
		std::cout << text << ": " << time << "s" << std::endl;
	}

	double getAverageTime()
	{
		if(++frames>0)
		{
			timeSum += getCurrentTime();
		}

		return timeSum / frames;
	}

	void printAverageTime(const char* text)
	{
		double avg = getAverageTime();
		if(frames>0)
		{
			std::cout << text << ": " << avg << "s" << std::endl;
		}
		else
		{
			std::cout << text << ": <waiting>" << std::endl;
		}
	}
};


#endif
