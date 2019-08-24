//
// Description: 
//
//
// Author: Christian Woizischke
//
//
#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

//#include <GL/glut.h>
#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include "HashMap.hpp"

#include "MultiThreading.hpp"	// for TestResults initialization
#include "XHierarchyConfig.hpp"	// for TestResults initialization

#include "Camera.hpp"
#include "CameraController.hpp"
#include "SimpleScene.hpp"
#include "IOpenGLImage.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "ShadowRay.hpp"
#include "RefxxctionRay.hpp"

#include "TimeMeasurement.hpp"
#include "TestSetup.hpp"
#include "TestResult.hpp"




class RayTracer
{
public:
	enum MODE
	{
		TEST,
		INTERACTIVE,
		VIDEO
	};

private:
	IOpenGLImage* openglImage;
	int width;
	int height;
	Camera* camera;
	CameraController cameraController;
	float cameraSpeed;
	vector<Triangle> triangles;
	vector<Light*> lights;
	bool shadows;

	/// shadow rays are traversed in a separate pass to make Frustum Tracing possible
	#ifdef MULTITHREADING
		vector<ShadowRay> shadowRays[THREAD_COUNT];
	#else
		vector<ShadowRay> shadowRays;
	#endif

	/// reflection/refraction rays are traversed in a separate pass to make Frustum Tracing possible
	#ifdef MULTITHREADING
		vector<RefxxctionRay> refxxctionRaysA[THREAD_COUNT];
		vector<RefxxctionRay> refxxctionRaysB[THREAD_COUNT];
		vector<RefxxctionRay>* refxxctionRays[THREAD_COUNT];
	#else
		vector<RefxxctionRay> refxxctionRaysA;
		vector<RefxxctionRay> refxxctionRaysB;
		vector<RefxxctionRay>* refxxctionRays;
	#endif

	Scene* scene;
	float sceneSize;	// max aabb extend of scene
	int currentMethod;
	std::vector<SCENE_TYPE> methods;
	MODE mode;
	TestSetup testSetup;
	TestResult testResult;
	bool background;	// show colored background instead of black
	unsigned long frameCounter;	// counts frames. for automatic test mode
	unsigned long framesPerTest;	// if frameCounter == framesPerTest then proceed with next test
	int currentModelFile;
	int previousModelFile;
	int modelFileCount;
	bool skybox;	// skybox for reflection/refraction. set by command line argument.
	char** modelFiles;
	vector<Material*> materials;
	vector<Image*> textures;
	TimeMeasurement traversalTimeMeasurement;
	TimeMeasurement raytraceTimeMeasurement;
	TimeMeasurement displayTimeMeasurement;

	RayTracer();

	void changeSize(GLsizei w, GLsizei h);
	void idle();
	void keyboard(unsigned char key, int , int );
	void mouse(int button, int state, int x, int y);
	void mouseMotion(int x, int y);

	/// callbacks for glut
	static void changeSizeCallback(GLsizei w, GLsizei h);
	static void idleCallback();
	static void keyboardCallback(unsigned char key, int , int );
	static void mouseCallback(int button, int state, int x, int y);
	static void mouseMotionCallback(int x, int y);
	static void renderCallback();

	void init(int argc, char** argv);
	void initGL(int argc, char** argv);
	void createImage(int argc, char** argv);
	void render();
	void shutdown();

	SceneConstructionDetails createScene(SCENE_TYPE type);
	void prepareRaytracing();
	void printTestResults();

public:
	static RayTracer& getInstance();
	
	void castShadowRay(ShadowRay& sray);
	void castRefxxctionRay(RefxxctionRay& sray);
	void castRefxxctionRay(vector<RefxxctionRay>& refxxctionRays);

	void run(int argc, char** argv, unsigned int width, unsigned int height);

	/// creates a new shadow ray and enqueues it in the renderer
	void createShadowRay(const qvec& origin, const qvec& direction, const qfloat& length, const qvec& color, const quad<vec*>& destination);

	/// creates a new shadow ray and traces it immediately
	void castShadowRay(const qvec& origin, const qvec& direction, const qfloat& length, const qvec& color, const quad<vec*>& destination);

	/// creates a new reflection/refraction ray and enqueues it in the renderer
	void createRefxxctionRay(const qvec& origin, const qvec& direction, const quad<vec*>& destination, int level, const qvec& contribution);

	/// creates a new reflection/refraction ray and traces it immediately
	void castRefxxctionRay(const qvec& origin, const qvec& direction, const quad<vec*>& destination, int level, const qvec& contribution);
};

#endif

