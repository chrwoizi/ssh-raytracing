/**
* \file CameraController.h
*
* \brief Declaration for class CameraController
*/

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <cmath>
#include <string>

//#include <GL/glew.h>
#include <GL/glut.h>

//#include <algebra.h>

#include "vecmath.h"

#include "Camera.hpp"

using std::string;

struct CameraControllerState
{
	float dist;
	float theta;
	float phi;
	float oldX;
	float oldY;  
	vec pos;
	vec lookAt;
	vec up;
	float zoomSpeed;
};

// adapted from
/*
* \class Trackball
* \brief Simple trackball system for camera rotation
*
* Class represents a trackball system for updating a camera view, the lookAt point is always (0,0,0) at the moment
*
* 
* \author 
* - Martin Eisemann (ME), eisemann@cg.cs.tu-bs.de
*
* \date   
* - Created 2006-09-12 (ME)
*
* \todo
* - change of viewpoint
* - change of upVector
* - polar2xyz only calculates position
*/

#ifndef M_PI
#define M_PI 3.141592
#endif

class CameraController
{
public:
	CameraController(float t = M_PI / 2.0f, float p = M_PI/2.0f, float d = 2.5f);
	void set(float theta = M_PI / 2.0f, float phi = M_PI/2.0f, float distance = 2.5f);
	void setPosition(vec v) { assert(mode == FirstPerson); state.pos = v; }
	void move(vec v);

	enum Mode
	{
		Trackball,
		FirstPerson
	};
	Mode mode;

	void mouseMotion(int x, int y); 
	void mouse(int button, int state, int x, int y);

	void apply(Camera& cam)
	{
		vec dir = getDir();
		normalize(dir);
		cam.set(state.pos, dir, vec(0,1,0));
	}

	vec getDir() const { return (mode==FirstPerson) ? state.lookAt : (state.lookAt-state.pos); }
	vec getPosition() const;
	vec getLookAt() const { return state.lookAt; };
	void getPolar(float& dist, float& theta, float& phi) const;
	float getPhi() const{ return state.phi; };

	void setDistance(float distance);
	void setZoomSpeed(float speed);

	void setLookAt(const vec lookAt){ assert(mode == Trackball); state.lookAt = lookAt; };

	void zoom(const float z);
	void rotateLeftRight(const float r);
	void rotateUpDown(const float r);

	void save();
	void load();

	void saveToFile(const char* filename) const;
	void loadFromFile(const char* filename);

	void loadGoal(const char* filename);
	void interpolateToGoal(float time);

private:
	unsigned int xRes, yRes;
	float fovy;

	void polar2xyz();
	/**
	* \brief Enumeration for depicting the current motion state
	*/
	enum motionStateEnum {ROTATE, /*!< CameraController is in rotation mode */
		MOVE,   /*!< CameraController is in move mode */
		STRAFE};  

	/**
	* \brief State variable depicting the current motion state
	*/
	motionStateEnum mMotionState;

	/**
	* \brief Current state
	*/
	CameraControllerState state;

	/**
	* \brief saves the old position
	*/
	CameraControllerState mOldState;

	/**
	* \brief target position
	*/
	CameraControllerState mGoalState;
};

#endif
