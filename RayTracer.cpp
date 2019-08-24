//
// Description:
//
//
// Author: Christian Woizischke
//
//

#include "RayTracer.hpp"

#include <iostream>
#include <vector>
#include <string.h>
#include <assert.h>

#include "MultiThreading.hpp"

#ifndef WINDOWS
#define CPP_PLYLOADER
#endif
#ifdef CPP_PLYLOADER
	#include "PlyloaderWrapper.hpp"
#else
	#include "PLYLoader.hpp"
#endif
#include "ModelParser.hpp"

#include "Ray.hpp"
#include "vecmath.h"

#include "kdTree.hpp"
#include "kdSpatialMedianCut.hpp"
#include "kdSurfaceAreaHeuristic.hpp"

#include "XHierarchy.hpp"
#include "XHierarchySpatialMedianCut.hpp"

#include "EyelightColorMaterial.hpp"
#include "PhongColorMaterial.hpp"
#include "PhongDiffuseTextureMaterial.hpp"
#include "SkyboxMaterial.hpp"

#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "DiscLight.hpp"
#include "QuadLight.hpp"

#include "OpenGLTexture.hpp"
#include "OpenGLDrawPixels.hpp"
#include "PBO.hpp"

#include "TestConfig.hpp"

#include "SimpleScene.hpp" // debug threads

using namespace std;

#define MAX(a,b) ((a)>(b)?(a):(b))

/// max number of reflections & refractions per primary ray
#define LEVELS 8
#define SKYBOX_SIZE 2000

Material* skyboxMaterial;

bool makeStats = true;
bool ignoreMaterials = false;

// to detect scenes in which the ssh is faster than the bvh
double sshMinTraversalTime = 10000;
double bvhMinTraversalTime = 10000;
double sshMinConstructionTime = 10000;
double bvhMinConstructionTime = 10000;

/// returns a command line argument by name
/// i.e. command line: "-answer=42"  ->  result = "42"
const char* getArgument(int argc, char** argv, const char* name)
{
	int arg = -1;
	for(int i = 1; i < argc; ++i)
	{
		if(!strncmp(argv[i], name, strlen(name))) {arg = i; break;}
	}

	if(arg >= 0) return &argv[arg][strlen(name)+1];
	else return NULL;
}

/// finds the first model file argument in command line args
int getFirstModelArgument(int argc, char** argv)
{
	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] != '-') {return i;}
	}
	return -1;
}

void printUsageAndExit()
{
	std::cout << "\n\nUsage:\n"
		<< "./simdtrace [-mode=<mode>] [-cameraMode=<cameraMode>] [-frames=<frames>] [-methods=<methods>] [-displayMethod=<displaymethod>] [-resolution=<resolution>] [-shadows=0|1] [-light=1|2|3|3] [-ignoreMaterials] [-nostats] <models> [<models>]...\n\n"
		<< "methods:\n"
		<< "K: KD Tree\n"
		<< "V: BVH - Bounding Volume Hierarchy\n"
		<< "S: SSH - Single Slab Hierarchy\n"
		<< "N: No acceleration method\n"
		<< "you can set multiple methods for test mode. e.g. -methods=SV\n\n"
		<< "camera modes:\n"
		<< "T: Trackball\n"
		<< "other or none: First person camera\n\n"
		<< "displayMethod:\n"
		<< "P: Pixel Buffer Object\n"
		<< "T: glTexture2D (default)\n"
		<< "D: glDrawPixels\n\n"
		<< "resolution is given as WIDTHxHEIGHT, e.g. 800x600\n\n"
		<< "model is the name of a model file, e.g. barney.obj.\n"
		<< "file extension must be obj or ply.\n"
		<< "you can run test with different model files by separating them with a \" \".\n"
		<< "you can load multiple files per test or for interactive mode by separating them with a \",\"\n"
		<< " or by saving them to a txt-file and using this file as model name.\n"
		<< "e.g. \"./simdtrace -mode=I -methods=S cow.obj,barney.obj\" will merge cow and barney into one scene\n\n"
		<< "mode:\n"
		<< "T: test mode\n"
		<< "I: interactive mode\n"
		<< "V: video mode. saves a series of images of the given models to disk.\n\n"
		<< "frames: number of frames per test run. used in test mode only.\n\n"
		<< "light: 1-6\n"
		<< " 1: far point light\n"
		<< " 2: white light front, colored light background\n"
		<< " 3: directional light\n"
		<< " 4: visualize surface normals\n"
		<< " 5: random sampled disc light\n"
		<< " 6: uniform sampled quad light\n\n"
		<< "ignoreMaterials: relpace materials by white eyelight shader\n\n"
		<< "noStats: disable measurements for test results\n\n"
		<< "keys:\n"
		<< "wsad: move camera\n"
		<< " i: load camera position\n"
		<< " o: save camera position\n"
		<< " b: turn on colored background\n"
		<< " p: save image to file\n\n";
	exit(1);
}

RayTracer::RayTracer()
{
	openglImage = 0;
	width = 1;
	height = 1;
	camera = 0;
	cameraSpeed = 0.1f;
	scene = 0;
	currentMethod = 0;
	mode = RayTracer::TEST;
	background = false;
	frameCounter = 0;
	framesPerTest = 1;
	currentModelFile = 0;
	previousModelFile = -1;
	modelFileCount = 0;
	modelFiles = NULL;
	makeStats = true;
}

RayTracer& RayTracer::getInstance()
{
	static RayTracer instance;
	return instance;
}

/// main loop entry
void RayTracer::run ( int argc, char** argv, unsigned int width, unsigned int height )
{
	const char* arg = getArgument(argc, argv, "-resolution");
	if(arg)
	{
		if(2 != sscanf(arg, "%dx%d", &this->width, &this->height))
		{
			this->width = width;
			this->height = height;
		}
	}
	else
	{
		this->width = width;
		this->height = height;
	}

	init(argc, argv);

	glutMainLoop();

	shutdown();
}

/// called whenever there is nothing else to do
void RayTracer::idle()
{
	// render new frame
	glutPostRedisplay();
}

void RayTracer::changeSizeCallback(GLsizei w, GLsizei h)
{
	RayTracer::getInstance().changeSize(w,h);
}

void RayTracer::idleCallback()
{
	RayTracer::getInstance().idle();
}

void RayTracer::keyboardCallback(unsigned char key, int a, int b)
{
	RayTracer::getInstance().keyboard(key, a, b);
}

void RayTracer::mouseCallback(int button, int state, int x, int y)
{
	RayTracer::getInstance().mouse(button, state, x, y);
}

void RayTracer::mouseMotionCallback(int x, int y)
{
	RayTracer::getInstance().mouseMotion(x, y);
}

void RayTracer::renderCallback()
{
	RayTracer::getInstance().render();
}

/// initializes OpenGL
void RayTracer::initGL(int argc, char** argv)
{
	// initialize the opengl window
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("RayTracer");

	glutIdleFunc(idleCallback);
	glutKeyboardFunc(keyboardCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);
	glutReshapeFunc(changeSizeCallback);
	glutDisplayFunc(renderCallback);

	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		// glewInit failed, something is seriously wrong.
		cout << "Error: " << glewGetErrorString(err) << endl;
	}

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
}

/// creates result image and on-screen output method
void RayTracer::createImage(int argc, char** argv)
{
	const char* arg = getArgument(argc, argv, "-displayMethod");

	if(arg)
	{
		if(arg[0] == 'P')
		{
			if(PBO::supportedByDevice())
			{
				cout<<"PBO chosen"<<endl;
				openglImage = new PBO(width, height, true);
			}
			else
			{
				cout<<"glTexture2D chosen"<<endl;
				openglImage = new OpenGLTexture(width, height);
			}
		}
		else if(arg[0] == 'T')
		{
			cout<<"glTexture2D chosen"<<endl;
			openglImage = new OpenGLTexture(width, height);
		}
		else if(arg[0] == 'D')
		{
			cout<<"glDrawPixels chosen"<<endl;
			openglImage = new OpenGLDrawPixels(width, height);
		}
		else
		{
			std::cout << "unknown display method" << endl;
			exit(-1);
		}
	}
	else
	{
		cout<<"glTexture2D chosen"<<endl;
		openglImage = new OpenGLTexture(width, height);
	}
}

/// initializes the window and the scene
void RayTracer::init(int argc, char** argv)
{
	// check arguments
	if(argc == 1)
	{
		printUsageAndExit();
	}

	initGL(argc, argv);

	createImage(argc, argv);

	// check for correct pointer size
	assert(sizeof(void*) == sizeof(unsigned long));

	// init camera
	
	const char* cameraMode = getArgument(argc, argv, "-cameraMode");
	if(cameraMode && cameraMode[0] == 'T')
	{
		cameraController.mode = CameraController::Trackball;
	}
	else
	{
		cameraController.mode = CameraController::FirstPerson;
	}

	if(cameraController.mode == CameraController::Trackball)
	{
		cameraController.set(1.0, 1.6, 5);
		cameraController.setLookAt(vec(0,0,0));
	}
	else
	{
		cameraController.set(1.6, -1.6, 5);
		cameraController.setPosition(vec(0, 0, 1));
	}

	vec dir = cameraController.getDir(); normalize(dir);
	camera = new Camera(width, height, 60.0f, cameraController.getPosition(), dir, vec(0.0f, 1.0f, 0.0f));

	// test material
	skyboxMaterial = new SkyboxMaterial();

	// acceleration structure
	const char* arg = getArgument(argc, argv, "-methods");
	if(arg)
	{
		for(unsigned int i = 0; i < strlen(arg); ++i)
		{
			// specific acceleration structure chosen
			switch(arg[i])
			{
			case 'K':
				methods.push_back(KD);
			break;
			case 'V':
				methods.push_back(BVH);
			break;
			case 'S':
				methods.push_back(SSH);
			break;
			case 'N':
				methods.push_back(SIMPLE);
			break;
			default:
				std::cout << "unknown acceleration method: " << arg[i] << endl;
				exit(-1);
			}
		}
	}
	else
	{
		methods.push_back(SSH);
	}

	// test mode or interactive mode?
	const char* moarg = getArgument(argc, argv, "-mode");
	if(moarg)
	{
		switch(moarg[0])
		{
		case 'T': mode = TEST; break;
		case 'I': mode = INTERACTIVE; break;
		case 'V': mode = VIDEO; break;
		}
	}
	else
	{
		mode = INTERACTIVE;
	}

	// multiple frames per test
	const char* farg = getArgument(argc, argv, "-frames");
	if(farg)
	{
		framesPerTest = atoi(farg);
	}

	// shadows enabled
	shadows = true;
	const char* sarg = getArgument(argc, argv, "-shadows");
	if(sarg)
	{
		shadows = atoi(sarg) > 0;
	}

	// materials disabled
	ignoreMaterials = getArgument(argc, argv, "-ignoreMaterials") != NULL;

	// measurements enabled
	makeStats = getArgument(argc, argv, "-nostats") == NULL;

	// shadows enabled
	const char* larg = getArgument(argc, argv, "-light");
	if(larg)
	{
		int light = atoi(larg);

		switch(light)
		{
		default: 
		case 1:
			// far point light
			lights.push_back(new PointLight(vec(1.f), vec(1000, 1000, 1000)));
			break;

		case 2:
			// white light front, colored light background
			lights.push_back(new DirectionalLight(vec(0.3f,0.3f,0.f), vec(-1, -1, 1)));
			lights.push_back(new DirectionalLight(vec(0.3f,0.f,0.f), vec(1, -1, -1)));
			lights.push_back(new DirectionalLight(vec(0.8f), vec(0, -1, 0)));
			break;

		case 3:
			// directional light
			lights.push_back(new DirectionalLight(vec(1.0f), vec(-1, -1, -1)));
			break;
		
		case 4:
			// visualize surface normals
			lights.push_back(new DirectionalLight(vec(1.f,0.f,0.f), vec(-1, 0, 0)));
			lights.push_back(new DirectionalLight(vec(0.f,1.f,0.f), vec(0, -1, 0)));
			lights.push_back(new DirectionalLight(vec(0.f,0.f,1.f), vec(0, 0, -1)));
			break;
		
		case 5:
			// random sampled disc light
			lights.push_back(new DiscLight(vec(1.f), vec(500,1000,0), 100, vec(-0.5f,-1,0), vec(0,0,1)));
			break;
		
		case 6:
			// uniform sampled quad light
			lights.push_back(new QuadLight(vec(1.f), vec(250,500,0), 200, 200, vec(-0.5f,-1,0), vec(0,0,1)));
			//lights.push_back(new QuadLight(vec(1.f), vec(250,-500,0), 200, 200, vec(-0.5f,1,0), vec(0,0,1))); // light setting for utah hand
			break;
		}
	}
	else
	{
		// far point light
		lights.push_back(new PointLight(vec(1.f), vec(1000, 1000, 1000)));
	}

	// skybox for reflection/refraction
	const char* skycmd = getArgument(argc, argv, "-skybox");
	skybox = skycmd != NULL;

	// model files
	int marg = getFirstModelArgument(argc, argv);
	modelFiles = &argv[marg];
	modelFileCount = argc-marg;
	currentModelFile = 0;
	if(currentModelFile == -1) printUsageAndExit();

	#ifdef MULTITHREADING
		for (int thread = 0; thread < THREAD_COUNT; ++thread)
		{
			refxxctionRaysA[thread].reserve(width*height);
			refxxctionRaysB[thread].reserve(width*height);
			refxxctionRays[thread] = &refxxctionRaysA[thread];
		}
	#else
		refxxctionRaysA.reserve(width*height);
		refxxctionRaysB.reserve(width*height);
		refxxctionRays = &refxxctionRaysA;
	#endif

	// start first test
	prepareRaytracing();
}

const char* getMethodStr(SCENE_TYPE type)
{
	switch(type)
	{
		case KD: return "kd-tree";
		case BVH: return "BVH";
		case SSH: return "SSH";
		case SIMPLE: return "none";
		default: return "unknown";
	}
}

/// model file names can be given as text file or separated by comma.
/// this function makes a vector of filenames by splitting the command line argument or parsing the text files
void splitModelFileNames(const char* fileNames, std::vector< std::pair<const char*, size_t> > &result)
{
	static char buffer[100000];
	static size_t bufferPos = 0;

	size_t fileNamesLen = strlen(fileNames);
	if(fileNamesLen > 4 && (fileNames[fileNamesLen-1] == 't') && (fileNames[fileNamesLen-2] == 'x') && (fileNames[fileNamesLen-3] == 't') && (fileNames[fileNamesLen-4] == '.'))
	{
		// extract path from fileNames
		char path[260];
		strcpy(path, fileNames);
		size_t pathLen = strlen(path);
		for(int i = pathLen-2; i >= 0; --i)
		{
			if(path[i] == '/')
			{
				path[i+1] = 0;
				break;
			}
		}
		pathLen = strlen(path);
		
		// get names from txt file
		FILE* f = fopen(fileNames, "rt");
		if(f != NULL)
		{
			char ln[260];
			bufferPos = 0;
			while(!feof(f))
			{
				if(1 == fscanf(f, "%s\n", ln) && ln[0] != 0)
				{
					size_t filepathBegin = bufferPos;
					
					// add path
					memcpy(&buffer[bufferPos], path, pathLen);
					bufferPos += pathLen;
					
					// add filename
					memcpy(&buffer[bufferPos], ln, strlen(ln));
					bufferPos += strlen(ln);
					
					result.push_back(std::pair<const char*,size_t>(&buffer[filepathBegin], strlen(ln) + pathLen));
				}
			}
			fclose(f);
		}
	}
	else
	{
		const char* current = fileNames;
		while(current)
		{
			const char* comma = strstr(current, ",");
			if(comma)
			{
				if(comma-current > 0)
				{
					result.push_back(std::pair<const char*,size_t>(current, comma-current));
				}
				current = comma+1;
			}
			else
			{
				result.push_back(std::pair<const char*,size_t>(current, strlen(current)));
				break;
			}
		}
	}
}

/// skybox for reflection/refraction
void createSkybox(vector<Triangle>& out)
{
	vec a(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	vec b(-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE);
	vec c(-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE);
	vec d(-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE);
	vec e( SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE);
	vec f( SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE);
	vec g( SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE);
	vec h( SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE);

	vec na( 1,  1,  1); normalize(na);
	vec nb( 1,  1, -1); normalize(nb);
	vec nc( 1, -1,  1); normalize(nc);
	vec nd( 1, -1, -1); normalize(nd);
	vec ne(-1,  1,  1); normalize(ne);
	vec nf(-1,  1, -1); normalize(nf);
	vec ng(-1, -1,  1); normalize(ng);
	vec nh(-1, -1, -1); normalize(nh);

	// top
	out.push_back(Triangle(skyboxMaterial, c, d, g, nc, nd, ng, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, d, h, g, nd, nh, ng, 0, 0, 0));

	// bottom
	out.push_back(Triangle(skyboxMaterial, a, e, b, na, ne, nb, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, b, e, f, nb, ne, nf, 0, 0, 0));

	// right
	out.push_back(Triangle(skyboxMaterial, g, f, e, ng, nf, ne, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, f, g, h, nf, ng, nh, 0, 0, 0));

	// left
	out.push_back(Triangle(skyboxMaterial, a, b, c, na, nb, nc, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, b, d, c, nb, nd, nc, 0, 0, 0));

	// front
	out.push_back(Triangle(skyboxMaterial, a, c, e, na, nc, ne, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, c, g, e, nc, ng, ne, 0, 0, 0));

	// back
	out.push_back(Triangle(skyboxMaterial, b, f, d, nb, nf, nd, 0, 0, 0));
	out.push_back(Triangle(skyboxMaterial, f, h, d, nf, nh, nd, 0, 0, 0));
}

/// test setup: load model, set camera, reset test results
void RayTracer::prepareRaytracing()
{
	if(currentModelFile != previousModelFile)
	{
		// delete old materials
		for(size_t i = 0; i < materials.size(); ++i)
		{
			delete materials[i];
		}
		materials.clear();
	
		//materials.push_back(new PhongColorMaterial(vec(0.0f), vec(0.0f), vec(0.f), 50.f, true, true, 5.5));

		// delete old textures
		for(size_t i = 0; i != textures.size(); ++i)
		{
			delete textures[i];
		}
		textures.clear();

		previousModelFile = currentModelFile;
		const char* modelFileList = modelFiles[currentModelFile];

		// when using model batches, the filenames are separated by ",".
		std::vector< std::pair<const char*,size_t> > fileNames;
		splitModelFileNames(modelFileList, fileNames);
		
		// load geometry
		triangles.clear();

		// need triangle count. see comment of next code block (triangles.reserve...)
		unsigned long triangleCount = 0;
		for(size_t i = 0; i < fileNames.size(); ++i)
		{
			string modelFile;
			char modelFileC[260];
			strncpy(modelFileC, fileNames[i].first, fileNames[i].second); 
			modelFileC[fileNames[i].second] = 0;
			modelFile = modelFileC;
			
			std::cout << "Getting triangle count of " << modelFile << endl;
			
			if(modelFile.find(".obj") != string::npos)
			{
				ModelParser mp;
				triangleCount += mp.getFaceCount(modelFile);
			}
			else if(modelFile.find(".off") != string::npos)
			{
				ModelParser mp;
				triangleCount += mp.getFaceCount(modelFile);
			}
			else if(modelFile.find(".ply") != string::npos)
			{
				triangleCount += getFaceCountFromPly(modelFile.c_str());
			}
			else
			{
				std::cout << "unknown file type: " << modelFile << endl;
				exit(-1);
			}
		}

		// vectors allocate double of the current size when using push_back on full vectors
		// so beginning with an empty vector could result in a size of 2*n-2 for n triangles.
		// avoid this by pre allocating the needed memory.
		triangles.reserve(triangleCount);

		for(size_t i = 0; i < fileNames.size(); ++i)
		{
			string modelFile;
			char modelFileC[260];
			strncpy(modelFileC, fileNames[i].first, fileNames[i].second); 
			modelFileC[fileNames[i].second] = 0;
			modelFile = modelFileC;

			// get directory of model file.
			string directory;
			size_t pos = modelFile.find_last_of("\\");
			if(pos == string::npos)
			{
				pos = modelFile.find_last_of("/");
				if(pos != string::npos)
				{
					directory = modelFile.substr(0, pos+1);
				}
			}
			else
			{
				size_t pos2 = modelFile.find_last_of("/");
				if(pos2 == string::npos)
				{
					directory = modelFile.substr(0, pos+1);
				}
				else
				{
					directory = modelFile.substr(0, (pos<pos2?pos:pos2)+1);
				}
			}


			std::cout << "Loading " << modelFile << endl;

			if(modelFile.find(".obj") != string::npos)
			{
				ModelParser mp;
				mp.loadFile(modelFile.c_str(), true);
				const vector<vec>& verts = mp.getVertices();
				const vector<int3>& tris = mp.getTriangles();
				const vector<vec>& norm = mp.getVertexNormals();
				const vector<vec>& texc = mp.getTextureCoordinates();
				const HashMap<int,int>& matids = mp.getMaterialIds();

				// create materials
				const vector<MtlMaterial>& mtls = mp.getMaterials();
				for(size_t i = 0; i < mtls.size(); ++i)
				{
					if(ignoreMaterials)
					{
						materials.push_back(new EyeLightColorMaterial(vec(1,1,1)));
					}
					else
					{
						bool hasReflection = mtls[i].illum >= 3 && mtls[i].illum <= 7;
						bool hasRefraction = mtls[i].illum >= 4 && mtls[i].illum <= 7;
						if(!mtls[i].map_Kd.empty())
						{
							string imgname = mtls[i].map_Kd;
							#if 0
								// some jpg files of the fairy model fail to load. use bmp versions of those files instead.
								size_t pos = imgname.find(".jpg");
								if(pos != string::npos) imgname.replace(pos, 4, ".bmp");
							#endif

							string imgpath = directory + imgname;

							Image* img = NULL;
							for(size_t t = 0; t < textures.size(); ++t)
							{
								if(textures[t]->getFileName() == imgpath)
								{
									img = textures[t];
									break;
								}
							}
							if(!img)
							{
								img = new Image();
								img->read(imgpath.c_str());
								textures.push_back(img);
							}
							materials.push_back(new PhongDiffuseTextureMaterial(mtls[i].Ka, *img, mtls[i].Ns > 0 ? mtls[i].Ks : vec(0,0,0), mtls[i].Ns, hasReflection, hasRefraction, mtls[i].Ni, vec(1,1,1), mtls[i].Tf));
						}
						else
						{
							materials.push_back(new PhongColorMaterial(mtls[i].Ka, mtls[i].Kd, mtls[i].Ns > 0 ? mtls[i].Ks : vec(0,0,0), mtls[i].Ns, hasReflection, hasRefraction, mtls[i].Ni, vec(1,1,1), mtls[i].Tf));
						}
					}
				}

				if(materials.empty())
				{
					// create dummy material
					materials.push_back(new PhongColorMaterial(vec(0.9f), vec(0.9f), vec(1.f), 50.f));
				}

				// add triangles to scene
				for(unsigned long i = 0; i < tris.size(); ++i)
				{
					HashMap<int,int>::const_iterator m = matids.find(i);
					int matid = (m != matids.end()) ? m->second : 0;
					if(matid >= (int)materials.size()) matid = 0;
					unsigned long id0 = tris[i].data[0];
					unsigned long id1 = tris[i].data[1];
					unsigned long id2 = tris[i].data[2];
					vec texca = id0 < texc.size() ? texc[id0] : vec(0,0,0);
					vec texcb = id1 < texc.size() ? texc[id1] : vec(0,0,0);
					vec texcc = id2 < texc.size() ? texc[id2] : vec(0,0,0);
					vec norma = id0 < norm.size() ? norm[id0] : vec(0,1,0);
					vec normb = id1 < norm.size() ? norm[id1] : vec(0,1,0);
					vec normc = id2 < norm.size() ? norm[id2] : vec(0,1,0);
					triangles.push_back(Triangle(
						materials[matid],
						verts[id0], verts[id1], verts[id2],
						norma, normb, normc,
						texca, texcb, texcc
					));
				}
			}
			else if(modelFile.find(".off") != string::npos)
			{
				ModelParser mp;
				mp.loadFile(modelFile.c_str(), true);
				const vector<vec>& verts = mp.getVertices();
				const vector<int3>& tris = mp.getTriangles();
				const vector<vec>& norm = mp.getVertexNormals();
				const vector<vec>& texc = mp.getTextureCoordinates();

				// create dummy material
				if(ignoreMaterials)
				{
					materials.push_back(new EyeLightColorMaterial(vec(1,1,1)));
				}
				else
				{
					materials.push_back(new PhongColorMaterial(vec(0.9f), vec(0.9f), vec(1.f), 50.f));
				}

				for(unsigned long i = 0; i < tris.size(); ++i)
				{
					unsigned long id0 = tris[i].data[0];
					unsigned long id1 = tris[i].data[1];
					unsigned long id2 = tris[i].data[2];
					vec texca = id0 < texc.size() ? texc[id0] : vec(0,0,0);
					vec texcb = id1 < texc.size() ? texc[id1] : vec(0,0,0);
					vec texcc = id2 < texc.size() ? texc[id2] : vec(0,0,0);
					vec norma = id0 < norm.size() ? norm[id0] : vec(0,1,0);
					vec normb = id1 < norm.size() ? norm[id1] : vec(0,1,0);
					vec normc = id2 < norm.size() ? norm[id2] : vec(0,1,0);
					triangles.push_back(Triangle(
						materials[0],
						verts[id0], verts[id1], verts[id2],
						norma, normb, normc,
						texca, texcb, texcc
					));
				}
			}
			else if(modelFile.find(".ply") != string::npos)
			{
				vector<vec> vertices;
				vector<vec> normals;
				vector<int3> tris;
				loadPly(modelFile.c_str(), vertices, normals, tris);

				// create dummy material
				if(ignoreMaterials)
				{
					materials.push_back(new EyeLightColorMaterial(vec(1,1,1)));
				}
				else
				{
					materials.push_back(new PhongColorMaterial(vec(0.9f), vec(0.9f), vec(1.f), 50.f));
				}
				
				for(unsigned long i = 0; i < tris.size(); ++i)
				{
					vec v1 = vertices[tris[i].data[0]];
					vec v2 = vertices[tris[i].data[1]];
					vec v3 = vertices[tris[i].data[2]];

					vec n1;
					vec n2;
					vec n3;
					#ifdef CPP_PLYLOADER	
						// c plyloader doesn't load the normals correctly. need to compute them ourselves.
						if(normals.size() > 0)
						{
							n1 = normals[tris[i].data[0]];
							n2 = normals[tris[i].data[1]];
							n3 = normals[tris[i].data[2]];
						}
						else
						{
							n1 = n2 = n3 = cross(v2-v1, v3-v1);
						}
					#else
						n1 = n2 = n3 = cross(v2-v1, v3-v1);
					#endif

					triangles.push_back(Triangle(
						materials[0],
						v1, v2, v3,
						n1, n2, n3,
						vec3(0,0,0),
						vec3(0,0,0),
						vec3(0,0,0)
					));
				}
			}
			else
			{
				std::cout << "unknown file type: " << modelFile << endl;
				exit(-1);
			}
		}

		testSetup.faceCount = triangles.size();
		if(triangles.empty())
		{
			std::cout << "No triangles found in given model files." << endl;
			exit(-1);
		}
	}

	// skybox for reflection/refraction
	if(skybox) createSkybox(triangles);

	if(makeStats)
	{
		testResult.clear();
		testSetup.clear();
	}

	// create scene
	SceneConstructionDetails details = createScene(methods[currentMethod]);

	const AABBox& sceneAABB = scene->getBounds();
	sceneSize = sceneAABB.max.x - sceneAABB.min.x;
	sceneSize = MAX(sceneSize, sceneAABB.max.y - sceneAABB.min.y);
	sceneSize = MAX(sceneSize, sceneAABB.max.z - sceneAABB.min.z);

	// find best camera settings
	if(!(mode == VIDEO && currentModelFile > 0))
	{
		/*if(cameraController.mode == CameraController::Trackball)
		{
			// find optimum camera position
			
			const AABBox& bbox = scene->getBounds();
			
			cameraController.setLookAt(0.5*(bbox.min + bbox.max));

			// zoom speed needs to be high for big scenes
			float boxwidth = bbox.max.x - bbox.min.x;
			float boxheight = bbox.max.y - bbox.min.y;
			float boxdepth = bbox.max.y - bbox.min.y;
			float sceneWidth = boxwidth > boxheight ? (boxwidth > boxdepth ? boxwidth : boxdepth) : (boxheight > boxdepth ? boxheight : boxdepth);
			cameraController.setZoomSpeed(0.001*sceneWidth);

			// from Camera::lookAt(AABBox)
			float boxaspect = boxwidth / boxheight;
			float dist;
			if (width/height >= boxaspect) {
				boxheight *= 1.05f;
				dist = (boxheight*0.5f)/tanf((0.5f*camera->getFov()*M_PI)/180.0f);
			} else {
				boxwidth *= 1.05f;
				dist = (boxwidth*0.5f)/tanf((float(width)/height*0.5f*camera->getFov()*M_PI)/180.0f);
			}
			cameraController.setDistance(bbox.max.z + dist);
			cameraController.apply(*camera);
		}
		else*/
		{
			// try to load camera settings from file
			char cameraFile[260];
			const char* modelFileName = modelFiles[currentModelFile];	// in video mode: use camera of first frame throughout whole video
			while(strstr(modelFileName, "/")) modelFileName = strstr(modelFileName, "/")+1;	// extract file name
			sprintf(cameraFile, "cameras/%s.camera", modelFileName);
			ifstream cam(cameraFile);
			if(!cam.fail())
			{
				cam.close();
				cameraController.loadFromFile(cameraFile);
				cameraController.apply(*camera);
			}
			else
			{
				// find optimum camera settings when not in video mode
				if(mode == TEST)
				{
					assert(camera);
					camera->lookAt(scene->getBounds());
				}
				else
				{
					// find optimum camera position
					
					const AABBox& bbox = scene->getBounds();

					// zoom speed needs to be high for big scenes
					float boxwidth = bbox.max.x - bbox.min.x;
					float boxheight = bbox.max.y - bbox.min.y;

					// from Camera::lookAt(AABBox)
					float boxaspect = boxwidth / boxheight;
					float dist;
					if (width/height >= boxaspect) {
						boxheight *= 1.05f;
						dist = (boxheight*0.5f)/tanf((0.5f*camera->getFov()*M_PI)/180.0f);
					} else {
						boxwidth *= 1.05f;
						dist = (boxwidth*0.5f)/tanf((float(width)/height*0.5f*camera->getFov()*M_PI)/180.0f);
					}
					cameraController.setPosition(vec(0,0,bbox.max.z + 2*dist));
					cameraController.apply(*camera);
				}
			}
		}
	}

	// camera speed depends on scene size
	const AABBox& aabb = scene->getBounds();
	float size = aabb.max.x - aabb.min.x;
	size = MAX(size, aabb.max.y - aabb.min.y);
	size = MAX(size, aabb.max.z - aabb.min.z);
	cameraSpeed = size * 0.01f;

	frameCounter = 0;

	cout << "Starting raytracing" << endl;
	
	if(makeStats)
	{
		testSetup.print(mode == TEST, details, scene->getComputedMemoryUsage(), modelFiles[currentModelFile], methods[currentMethod], getMethodStr(methods[currentMethod]), framesPerTest, scene, width, height);
	
		#if 0
			// print surface approximation to file
			if(methods[currentMethod] == SSH)
			{
				ofstream fil("testresults/surface.txt", ios_base::app);
				fil << modelFiles[currentModelFile] << " - average SSH node surface ratio approx/real: "
				#ifdef BIGFLOAT_SURFACE_COMPUTATION		
						<< details.bshSurfaceRatio.getdouble() << endl;
				#else
						<< details.bshSurfaceRatio << endl;
				#endif
				fil.close();
			}
		#endif

		// init measurements
		traversalTimeMeasurement.startAverageMeasurement(0);
		raytraceTimeMeasurement.startAverageMeasurement(0);
		displayTimeMeasurement.startAverageMeasurement(0);
	}
}

TimeMeasurement constructionTimeMeasurement;
SceneConstructionDetails RayTracer::createScene(SCENE_TYPE type)
{
	// delete scene, construction strategy and geometry
	if(scene) delete scene;

	switch(type)
	{
	case BVH:
		std::cout << "Creating BVH" << endl;
		scene = new BoundingVolumeHierarchy(new BoundingVolumeHierarchySpatialMedianCut());
	break;
	case SSH:
		std::cout << "Creating SSH" << endl;
		scene = new SingleSlabHierarchy(new SingleSlabHierarchySpatialMedianCut());
	break;
	case KD:
		std::cout << "kd-Trees are not supported yet" << endl; 
		exit(0);
		//std::cout << "\nCreating kdTree\n";
		//scene = new kdTree(new kdSpatialMedianCut);
	break;
	case SIMPLE:
		std::cout << "using no acceleration.\n";
		scene = new SimpleScene();
	break;
	}

	SceneConstructionDetails result;

	// construct
	if(makeStats)
	{
		constructionTimeMeasurement.restart();
		result = scene->construct(&triangles);
		testSetup.constructionTime = constructionTimeMeasurement.getCurrentTime();
	}
	else
	{
		result = scene->construct(&triangles);
	}

	return result;
}

void RayTracer::castShadowRay(ShadowRay& sray)
{
	if(shadows)
	{
		scene->intersect(*sray.ray);

		if((*sray.destination)[0] && !sray.ray->hit[0]) *(*sray.destination)[0] += vec(sray.color->x[0], sray.color->y[0], sray.color->z[0]);
		if((*sray.destination)[1] && !sray.ray->hit[1]) *(*sray.destination)[1] += vec(sray.color->x[1], sray.color->y[1], sray.color->z[1]);
		if((*sray.destination)[2] && !sray.ray->hit[2]) *(*sray.destination)[2] += vec(sray.color->x[2], sray.color->y[2], sray.color->z[2]);
		if((*sray.destination)[3] && !sray.ray->hit[3]) *(*sray.destination)[3] += vec(sray.color->x[3], sray.color->y[3], sray.color->z[3]);
	}
	else
	{
		if((*sray.destination)[0]) *(*sray.destination)[0] += vec(sray.color->x[0], sray.color->y[0], sray.color->z[0]);
		if((*sray.destination)[1]) *(*sray.destination)[1] += vec(sray.color->x[1], sray.color->y[1], sray.color->z[1]);
		if((*sray.destination)[2]) *(*sray.destination)[2] += vec(sray.color->x[2], sray.color->y[2], sray.color->z[2]);
		if((*sray.destination)[3]) *(*sray.destination)[3] += vec(sray.color->x[3], sray.color->y[3], sray.color->z[3]);
	}
	
	delete sray.color;
	delete sray.destination;
	delete sray.ray;
}

void RayTracer::castRefxxctionRay(RefxxctionRay& sray)
{
	scene->intersect(*sray.ray);

	quad<Triangle*> hit0(sray.ray->hit[0]);
	if((*sray.destination)[1] && (sray.ray->hit == hit0).allTrue())
	{
		// more than 1 destination -> packed ray
		// && all 4 rays hit the same triangle
		if((*sray.destination)[0] && sray.ray->hit[0]) sray.ray->hit[0]->material->shade(*sray.destination, *sray.contribution, sray.level, lights, *sray.ray);
	}
	else
	{
		// single rays or rays hit different triangles
		vec contribution0 = vec(sray.contribution->x[0], sray.contribution->y[0], sray.contribution->z[0]);
		vec contribution1 = vec(sray.contribution->x[1], sray.contribution->y[1], sray.contribution->z[1]);
		vec contribution2 = vec(sray.contribution->x[2], sray.contribution->y[2], sray.contribution->z[2]);
		vec contribution3 = vec(sray.contribution->x[3], sray.contribution->y[3], sray.contribution->z[3]);
		if((*sray.destination)[0] && sray.ray->hit[0]) sray.ray->hit[0]->material->shade(*(*sray.destination)[0], contribution0, sray.level, lights, *sray.ray, 0);
		if((*sray.destination)[1] && sray.ray->hit[1]) sray.ray->hit[1]->material->shade(*(*sray.destination)[1], contribution1, sray.level, lights, *sray.ray, 1);
		if((*sray.destination)[2] && sray.ray->hit[2]) sray.ray->hit[2]->material->shade(*(*sray.destination)[2], contribution2, sray.level, lights, *sray.ray, 2);
		if((*sray.destination)[3] && sray.ray->hit[3]) sray.ray->hit[3]->material->shade(*(*sray.destination)[3], contribution3, sray.level, lights, *sray.ray, 3);
	}
	
	delete sray.destination;
	delete sray.ray;
	delete sray.contribution;
}

void RayTracer::castRefxxctionRay(vector<RefxxctionRay>& refxxctionRays)
{
	for (int i = 0; i < (int)refxxctionRays.size(); ++i)
	{
		castRefxxctionRay(refxxctionRays[i]);
	}
	refxxctionRays.clear();
}

/**
* renders the scene
*/
void RayTracer::render()
{
	// clear screen
	glClear ( GL_COLOR_BUFFER_BIT );

	// initialize measurements
	if(makeStats)
	{
		Triangle::intersectionTestsPerformed = 0;
		testResult.rayNodeIntersections = 0;
	}

	PackedRay r;
	
	// prepare display method for color write
	assert(openglImage);
	if(makeStats)
	{
		displayTimeMeasurement.restart();
		openglImage->beginWrite();
		displayTimeMeasurement.pause();

		raytraceTimeMeasurement.restart();
		traversalTimeMeasurement.restart();
	}
	else
	{
		openglImage->beginWrite();
	}
	
	// clear image. secondary rays need to add their shading results to the image instead of just setting it. 
	// we could use the same algorithm for primary and secondary rays if the primary rays also added their result
	// instead of setting it. therefore we clean the image so that the primary rays can add their shading result to the
	// existing zeros.
#ifdef MULTITHREADING
	#pragma omp parallel for private(r) num_threads(THREAD_COUNT)
#endif
	for (int x = 0; x < width; x += 2)
	{
		for (int y = 0; y < height; y += 2)
		{
			static vec black(0,0,0);
			openglImage->setPixel(x,   y,   black);
			openglImage->setPixel(x+1, y,   black);
			openglImage->setPixel(x,   y+1, black);
			openglImage->setPixel(x+1, y+1, black);
		}
	}

	// private(r): do not share r between threads
#ifdef MULTITHREADING
	#pragma omp parallel for private(r) num_threads(THREAD_COUNT)
#endif
	for (int x = 0; x < width; x += 2)
	{
		for (int y = 0; y < height; y += 2)
		{
			assert(camera);
			camera->getRays(r, x, y);

			#ifndef MULTITHREADING
				if(makeStats)
				{
					// measurement is paused while shading. resume the measurement for upcoming traversal.
					traversalTimeMeasurement.resume();
				}
			#endif

			IntersectDetails details = scene->intersect(r);

			#ifndef MULTITHREADING
				if(makeStats)
				{
					// we do not want to measure the shading. pause until next traversal.
					traversalTimeMeasurement.pause();

					// TODO use atomic add operation to enable this measurement in multithreading mode without synchronization
					testResult.rayNodeIntersections += details.rayNodeIntersections;
				}
			#endif

			quad<Triangle*> hit0(r.hit[0]);
			if((r.hit == hit0).allTrue())
			{
				// all 4 rays hit the same triangle
				quad<vec*> destination(
					&(*openglImage)[y][x],
					&(*openglImage)[y][x+1],
					&(*openglImage)[y+1][x],
					&(*openglImage)[y+1][x+1]);

				if(r.hit[0]) r.hit[0]->material->shade(destination, qfloat(1.f), LEVELS, lights, r);
				else if(background)
				{
					// show ray direction in the background for debugging
					openglImage->setPixel(x,   y,   vec(r.dir.x[0], r.dir.y[0], r.dir.z[0]));
					openglImage->setPixel(x+1, y,   vec(r.dir.x[1], r.dir.y[1], r.dir.z[1]));
					openglImage->setPixel(x,   y+1, vec(r.dir.x[2], r.dir.y[2], r.dir.z[2]));
					openglImage->setPixel(x+1, y+1, vec(r.dir.x[3], r.dir.y[3], r.dir.z[3]));
				}
			}
			else
			{
				if(r.hit[0]) r.hit[0]->material->shade((*openglImage)[y][x], 1, LEVELS, lights, r, 0);
				if(r.hit[1]) r.hit[1]->material->shade((*openglImage)[y][x+1], 1, LEVELS, lights, r, 1);
				if(r.hit[2]) r.hit[2]->material->shade((*openglImage)[y+1][x], 1, LEVELS, lights, r, 2);
				if(r.hit[3]) r.hit[3]->material->shade((*openglImage)[y+1][x+1], 1, LEVELS, lights, r, 3);
			}
		}
	}

	// trace secondary rays
		
	for (int level = 0; level < LEVELS; ++level)
	{
		// shadow rays can be traversed recursively (instantly) or iteratively (after the primary rays).
		// the iterative version allows for further optimizations i.e. tracing ray bundles.
		// the recursive version uses less memory than the iterative version since there is no queue.
		#ifdef ITERATIVE_SHADOWS
			// trace shadow rays
			#ifdef MULTITHREADING
				// with multithreading each thread has its own queue to minimize synchronization
				for (int thread = 0; thread < THREAD_COUNT; ++thread)
				{
					#pragma omp parallel for private(r) num_threads(THREAD_COUNT)
					for (int i = 0; i < (int)shadowRays[thread].size(); ++i)
					{
						castShadowRay(shadowRays[thread][i]);
					}
					shadowRays[thread].clear();
				}
			#else
				for (int i = 0; i < (int)shadowRays.size(); ++i)
				{
					castShadowRay(shadowRays[i]);
				}
				shadowRays.clear();
			#endif
		#endif
		
		// trace reflection/refraction rays
		#ifdef MULTITHREADING
			// with multithreading each thread has its own queue to avoid synchronization
			#pragma omp parallel for private(r) num_threads(THREAD_COUNT)
			for (int thread = 0; thread < THREAD_COUNT; ++thread)
			{
				castRefxxctionRay(*refxxctionRays[thread]);
			}
		#else
			castRefxxctionRay(*refxxctionRays);
		#endif

		// swap refxxtionRays vectors. tracing reflection/refraction rays can produce more reflection/refraction rays which 
		// need to be stored in another queue, so there are two queues which are swapped when one queue is empty.
		// this wouldn't be neccessary if the queues were real queues (linked lists), but here we use vectors to reduce memory usage and increase cache coherency.
		#ifdef MULTITHREADING
			vector<RefxxctionRay>* readingRefxxctionRays[THREAD_COUNT];
			for (int thread = 0; thread < THREAD_COUNT; ++thread)
			{
				readingRefxxctionRays[thread] = refxxctionRays[thread];
				refxxctionRays[thread] = refxxctionRays[thread] == &refxxctionRaysA[thread] ? &refxxctionRaysB[thread] : &refxxctionRaysA[thread];
			}
		#else
			vector<RefxxctionRay>* readingRefxxctionRays = refxxctionRays;
			refxxctionRays = refxxctionRays == &refxxctionRaysA ? &refxxctionRaysB : &refxxctionRaysA;
		#endif
	}

	if(makeStats)
	{
		#ifndef MULTITHREADING
			// pure traversal time is measureable in single-thread mode only due to missing thread synchronization
			testResult.lastTraversalTime = traversalTimeMeasurement.getCurrentTime();
			testResult.avgTraversalTime = traversalTimeMeasurement.getAverageTime();
			if(testResult.minTraversalTime > testResult.lastTraversalTime) testResult.minTraversalTime = testResult.lastTraversalTime;
			if(testResult.maxTraversalTime < testResult.lastTraversalTime) testResult.maxTraversalTime = testResult.lastTraversalTime;
			if(testResult.firstTraversalTime < 0.0)
			{
				testResult.firstTraversalTime = testResult.lastTraversalTime;
			}
		#endif

		testResult.lastRayTraceTime = raytraceTimeMeasurement.getCurrentTime();
		testResult.avgRayTraceTime = raytraceTimeMeasurement.getAverageTime();
		if(testResult.minRayTraceTime > testResult.lastRayTraceTime) testResult.minRayTraceTime = testResult.lastRayTraceTime;
		if(testResult.maxRayTraceTime < testResult.lastRayTraceTime) testResult.maxRayTraceTime = testResult.lastRayTraceTime;
		if(testResult.firstRayTraceTime < 0.0)
		{
			testResult.firstRayTraceTime = testResult.lastRayTraceTime;
			if(mode != TEST) testResult.printFirstFrame();
		}

		#ifndef MULTITHREADING
			// ray/triangle intersections is measureable in single-thread mode only due to missing thread synchronization
			testResult.rayTriangleIntersections = Triangle::intersectionTestsPerformed;
		#endif
	}
	
	// download image to graphics card
	
	if(makeStats)
	{
		displayTimeMeasurement.resume();
		openglImage->endWrite();
		openglImage->drawFullscreen();
		displayTimeMeasurement.pause();
		//cout << "Display time: " << displayTimeMeasurement.getCurrentTime() << " ; avg: " << displayTimeMeasurement.getAverageTime() << endl;
	}
	else
	{
		openglImage->endWrite();
		openglImage->drawFullscreen();
	}

	// switch back and front buffer
	glutSwapBuffers();

	if(mode == TEST)
	{
		++frameCounter;
		if(frameCounter >= framesPerTest)
		{
			if(makeStats)
			{
				testResult.print(width, height);

				// to find out if ssh was faster than bvh
				if(methods[currentMethod] == SSH) sshMinTraversalTime = testResult.minTraversalTime < sshMinTraversalTime ? testResult.minTraversalTime : sshMinTraversalTime;
				if(methods[currentMethod] == BVH) bvhMinTraversalTime = testResult.minTraversalTime < bvhMinTraversalTime ? testResult.minTraversalTime : bvhMinTraversalTime;
				if(methods[currentMethod] == SSH) sshMinConstructionTime = testSetup.constructionTime < sshMinConstructionTime ? testSetup.constructionTime : sshMinConstructionTime;
				if(methods[currentMethod] == BVH) bvhMinConstructionTime = testSetup.constructionTime < bvhMinConstructionTime ? testSetup.constructionTime : bvhMinConstructionTime;
			}
			
			++currentMethod;
			if(currentMethod >= (int)methods.size())
			{		
				if(makeStats)
				{
					// tell which method was faster
					ofstream fil("testresults/speed.txt", ios_base::app);
					if(sshMinTraversalTime <= bvhMinTraversalTime) {
						fil << modelFiles[currentModelFile] << " - SSH traversal " << (bvhMinTraversalTime/sshMinTraversalTime) << "x as fast as BVH" << endl;
					}
					else {
						fil << modelFiles[currentModelFile] << " - BVH traversal " << (sshMinTraversalTime/bvhMinTraversalTime) << "x as fast as SSH" << endl;
					}
					if(sshMinConstructionTime <= bvhMinConstructionTime) {
						fil << modelFiles[currentModelFile] << " - SSH construction " << (bvhMinConstructionTime/sshMinConstructionTime) << "x as fast as BVH" << endl;
					}
					else {
						fil << modelFiles[currentModelFile] << " - BVH construction " << (sshMinConstructionTime/bvhMinConstructionTime) << "x as fast as SSH" << endl;
					}
					fil.close();
					// reset
					sshMinTraversalTime = 10000;
					bvhMinTraversalTime = 10000;
					sshMinConstructionTime = 10000;
					bvhMinConstructionTime = 10000;
				}
				
				// all methods tested with the current model file.
				currentMethod = 0;

				// load next model file
				++currentModelFile;
				if(currentModelFile >= modelFileCount) exit(0);
			}

			prepareRaytracing();
		}
	}
	else if(mode == INTERACTIVE)
	{
		if(makeStats)
		{
			// print only data that changes every frame
			testResult.printEveryFrame(width, height);
		}
	}
	else
	{
		// save image
		char filename[260];
		const char* modelFileName = modelFiles[currentModelFile];
		while(strstr(modelFileName, "/")) modelFileName = strstr(modelFileName, "/")+1;	// extract file name
		sprintf(filename, "images/%s.ppm", modelFileName);
		openglImage->saveToFile(filename);

		// load next model file
		++currentModelFile;
		if(currentModelFile >= modelFileCount) exit(0);
		prepareRaytracing();
	}
}

void RayTracer::shutdown()
{
	delete scene;
	delete camera;
	delete openglImage;
}

/// called whenever the window size changes
void RayTracer::changeSize ( GLsizei w, GLsizei h )
{
	// avoid division by zero
	if ( h==0 ) h = 1;

	glViewport ( 0, 0, w, h );

	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity();
	glOrtho ( 0.0f, 100.f, 0.0f, 100.f, 1.0, -1.0 );

	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity();
}

/// called whenever a key is pressed
void RayTracer::keyboard ( unsigned char key, int , int )
{
	// TODO for some reason this method isn't called for arrow-keys.
	//cout<<"key "<<(int)key<<endl;

	switch ( key )
	{
		case 38: // up key
		case 'w':
			cameraController.move(vec(0,0,-cameraSpeed));
			cameraController.apply(*camera);
			break;
		case 40: // down key
		case 's':
			cameraController.move(vec(0,0,cameraSpeed));
			cameraController.apply(*camera);
			break;
		case 37: // left key
		case 'a':
			cameraController.move(vec(-cameraSpeed,0,0));
			cameraController.apply(*camera);
			break;
		case 39: // right key
		case 'd':
			cameraController.move(vec(cameraSpeed,0,0));
			cameraController.apply(*camera);
			break;
		case 'o':
			{
				// save camera (o for 'out')
				char filename[260];
				const char* modelFileName = modelFiles[currentModelFile];
				while(strstr(modelFileName, "/")) modelFileName = strstr(modelFileName, "/")+1;	// extract file name
				sprintf(filename, "cameras/%s.camera", modelFileName);
				cameraController.saveToFile(filename);
			}
			break;
		case 'i':
			{
				// load camera (i for 'in')
				char filename[260];
				const char* modelFileName = modelFiles[currentModelFile];
				while(strstr(modelFileName, "/")) modelFileName = strstr(modelFileName, "/")+1;	// extract file name
				sprintf(filename, "cameras/%s.camera", modelFileName);
				ifstream file(filename);
				if(!file.fail()) 
				{
					file.close();
					cameraController.loadFromFile(filename);
					cameraController.apply(*camera);
				}
			}
			break;
		case 'b':
			// toggle scene background
			background = !background;
			break;
		case 'p':
		{
			// save image
			char filename[260];
			const char* modelFileName = modelFiles[currentModelFile];
			while(strstr(modelFileName, "/")) modelFileName = strstr(modelFileName, "/")+1;	// extract file name
			sprintf(filename, "images/%s.ppm", modelFileName);
			openglImage->saveToFile(filename);
		}
		break;
		case 27: // escape
			exit(0);
			break;
	}
}


/// called whenever a mouse button is pressed or released
void RayTracer::mouse ( int button, int state, int x, int y )
{
	if(mode != TEST)
	{
		cameraController.mouse(button, state, x, y);
	}
}


/// called whenever a button is pressed and the mouse is moved
void RayTracer::mouseMotion ( int x, int y )
{
	if(mode != TEST)
	{
		cameraController.mouseMotion(x, y);
		cameraController.apply(*camera);
	}
}

/// creates a new shadow ray and enqueues it in the renderer
void RayTracer::createShadowRay(const qvec& origin, const qvec& direction, const qfloat& length, const qvec& color, const quad<vec*>& destination)
{
	#ifdef MULTITHREADING
		// each thread owns an array to prevent threads from interfering each other
		int thread = omp_get_thread_num();
		shadowRays[thread].resize(shadowRays[thread].size()+1);
		ShadowRay& shadowRay = shadowRays[thread][shadowRays[thread].size()-1];
	#else
		shadowRays.resize(shadowRays.size()+1);
		ShadowRay& shadowRay = shadowRays[shadowRays.size()-1];
	#endif

	static qfloat BIAS(sceneSize / 100.f);

	// TODO use placement new operator with a preallocated heap to avoid thread synchronization and OS overhead

	shadowRay.destination = new quad<vec*>();
	*shadowRay.destination = destination;
	shadowRay.color = new qvec();
	*shadowRay.color = color;
	shadowRay.ray = new PackedRay();
	shadowRay.ray->origin = origin + direction * BIAS;
	shadowRay.ray->hit = NULL;
	shadowRay.ray->dir = direction;
	shadowRay.ray->dirrcp = qvec(qfloat(1.0f)) / direction;
	shadowRay.ray->t = length - BIAS;
}

/// creates a new shadow ray and enqueues it in the renderer
void RayTracer::castShadowRay(const qvec& origin, const qvec& direction, const qfloat& length, const qvec& color, const quad<vec*>& destination)
{
	static qfloat BIAS(sceneSize / 100.f);

	// TODO use placement new operator with a preallocated heap to avoid thread synchronization and OS overhead

	ShadowRay shadowRay;
	shadowRay.destination = new quad<vec*>();
	*shadowRay.destination = destination;
	shadowRay.color = new qvec();
	*shadowRay.color = color;
	shadowRay.ray = new PackedRay();
	shadowRay.ray->origin = origin + direction * BIAS;
	shadowRay.ray->hit = NULL;
	shadowRay.ray->dir = direction;
	shadowRay.ray->dirrcp = qvec(qfloat(1.0f)) / direction;
	shadowRay.ray->t = length - BIAS;

	castShadowRay(shadowRay);
}

/// creates a new reflection/refraction ray and enqueues it in the renderer
void RayTracer::createRefxxctionRay(const qvec& origin, const qvec& direction, const quad<vec*>& destination, int level, const qvec& contribution)
{
	#ifdef MULTITHREADING
		// each thread owns an array to prevent threads from interfering each other
		int thread = omp_get_thread_num();
		refxxctionRays[thread]->resize(refxxctionRays[thread]->size()+1);
		RefxxctionRay& refxxctionRay = (*refxxctionRays[thread])[refxxctionRays[thread]->size()-1];
	#else
		refxxctionRays->resize(refxxctionRays->size()+1);
		RefxxctionRay& refxxctionRay = (*refxxctionRays)[refxxctionRays->size()-1];
	#endif

	static qfloat BIAS(sceneSize / 500.f);

	refxxctionRay.destination = new quad<vec*>();
	*refxxctionRay.destination = destination;
	refxxctionRay.contribution = new qvec(contribution);
	refxxctionRay.level = level;
	refxxctionRay.ray = new PackedRay();
	refxxctionRay.ray->origin = origin + direction * BIAS;
	refxxctionRay.ray->hit = NULL;
	refxxctionRay.ray->dir = direction;
	refxxctionRay.ray->dirrcp = qvec(qfloat(1.0f)) / direction;
	refxxctionRay.ray->t = HUGE_VAL;
}

/// creates a new reflection/refraction ray and enqueues it in the renderer
void RayTracer::castRefxxctionRay(const qvec& origin, const qvec& direction, const quad<vec*>& destination, int level, const qvec& contribution)
{
	static qfloat BIAS(sceneSize / 500.f);

	RefxxctionRay refxxctionRay;
	refxxctionRay.destination = new quad<vec*>();
	*refxxctionRay.destination = destination;
	refxxctionRay.contribution = new qvec(contribution);
	refxxctionRay.level = level;
	refxxctionRay.ray = new PackedRay();
	refxxctionRay.ray->origin = origin + direction * BIAS;
	refxxctionRay.ray->hit = NULL;
	refxxctionRay.ray->dir = direction;
	refxxctionRay.ray->dirrcp = qvec(qfloat(1.0f)) / direction;
	refxxctionRay.ray->t = HUGE_VAL;

	castRefxxctionRay(refxxctionRay);
}
