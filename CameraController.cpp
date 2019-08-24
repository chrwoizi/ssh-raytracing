#include "CameraController.hpp"


#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using std::ifstream;
using std::ofstream;
using std::stringstream;

/**
* \brief Constructor
*
* \param t - initial theta
* \param p - initial phi
* \param d - initail distance
*/
CameraController::CameraController(float t, float p, float d)
{
	mode = Trackball;

	state.zoomSpeed = 0.01f;
	mMotionState = ROTATE;

	state.pos = vec(0, 0, 1);
	state.lookAt = vec(0,0,0);
	state.up = vec(0,1,0);
	
	set(t, p, d);

	polar2xyz(); // update data
}

void CameraController::set(float t, float p, float d)
{
	state.theta = t;
	state.phi = p;
	state.dist = d;

	state.oldX = 0;
	state.oldY = 0;

	polar2xyz(); // update data
}

void CameraController::move(vec v)
{ 
	assert(mode == FirstPerson); 
	vec dir = -getDir(); normalize(dir);
	vec right = cross(state.up, dir);
	state.pos += v.x * right + v.y * state.up + v.z * dir; 
}


/**
* \brief Tracks the motion from the last position and calcs changes
*
* \param x - mouse x position in window coordinates
* \param y - mouse y position in window coordinates
*/
void CameraController::mouseMotion(int x, int y)
{
	float deltaX = x - state.oldX;
	float deltaY = y - state.oldY;

	if(mode == FirstPerson) deltaY = -deltaY;

	if (mMotionState == ROTATE) {
		state.theta -= 0.01 * deltaY;

		if (state.theta < 0.01) state.theta = 0.01;
		else if (state.theta > M_PI - 0.01) state.theta = M_PI - 0.01;

		state.phi += 0.01 * deltaX;	
		if (state.phi < 0) state.phi += 2*M_PI;
		else if (state.phi > 2*M_PI) state.phi -= 2*M_PI;
	}
	else if (mMotionState == MOVE) {
		//state.dist += 0.01 * deltaY;
		state.dist += state.zoomSpeed * deltaY;
	}
	else if(mMotionState == STRAFE){
		vec right = cross(state.lookAt - state.pos, state.up);
		vec up = cross(state.lookAt - state.pos, right);
		state.lookAt -= 0.01f * deltaX * right;
		state.lookAt -= 0.01f * deltaY * up;
	}

	state.oldX = x;
	state.oldY = y;

	polar2xyz(); // update data
}


/**
* \brief activated function when a mouse button is pressed or released
*
* \param button - button pressed
* \param state - button pressed or released
* \param x - mouse x position in window coordinates
* \param y - mouse y position in window coordinates
*/
void CameraController::mouse(int button, int state, int x, int y)
{
	this->state.oldX = x;
	this->state.oldY = y;

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mMotionState = ROTATE;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mMotionState = MOVE;
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON) {
		if (state == GLUT_DOWN) {
			mMotionState = STRAFE;
		}
	}
} 


/**
* \brief Returns the currently saved position
*
* \param xyz - Reference to the xyz position
*/
vec CameraController::getPosition() const
{
	return state.pos;
}

/**
* \brief Returns the currently saved parameters als polar coordinates, only valid if lookAt is (0,0,0)
*/
void CameraController::getPolar(float& dist, float& theta, float& phi) const
{
	dist  = state.dist;
	theta = state.theta;
	phi   = state.phi;
}


/**
* \brief Calculates the world coordinates the position of the camera from its polar coordinates, later also the lookAt an up vector should be added
*
* \param pos - Handle to the position of the camera in world coordinates
*/
void CameraController::polar2xyz()
{
	if(mode == FirstPerson)
	{
		state.lookAt[0] = sin(state.theta) * cos(state.phi);
		state.lookAt[1] = cos(state.theta);
		state.lookAt[2] = sin(state.theta) * sin(state.phi);
	}
	else
	{
		state.pos[0] = state.dist * sin(state.theta) * cos(state.phi);
		state.pos[1] = state.dist * cos(state.theta);
		state.pos[2] = state.dist * sin(state.theta) * sin(state.phi);
	}
}


/**
* \brief Change the distance
*/ 
void CameraController::setDistance(float distance)
{
	state.dist = distance;
	polar2xyz(); // update data
}


/**
* \brief Change the zoom speed
*/
void CameraController::setZoomSpeed(float speed)
{
	state.zoomSpeed = speed;
}


/**
* \brief Zoom in or out
*/
void CameraController::zoom(const float z)
{
	state.dist += 2*z;
	if(state.dist < 0.0f){ state.dist = 0.0f; }

	polar2xyz();
}


/**
* \brief Rotate left or right
*/
void CameraController::rotateLeftRight(const float r)
{
	state.phi += r;
	if (state.phi < 0) state.phi += 2*M_PI;
	else if (state.phi > 2*M_PI) state.phi -= 2*M_PI;

	polar2xyz();
}


/**
* \brief Rotate left or right
*/
void CameraController::rotateUpDown(const float r)
{
	state.theta += r;
	if (state.theta < 0.01) state.theta = 0.01;
	else if (state.theta > M_PI - 0.01) state.theta = M_PI - 0.01;

	polar2xyz();
}



/**
* \brief save current state
*/
void CameraController::save()
{
	mOldState.dist = state.dist;
	mOldState.theta = state.theta;
	mOldState.phi = state.phi;
	mOldState.oldX = state.oldX;
	mOldState.oldY = state.oldY;
	mOldState.pos = state.pos;
	mOldState.lookAt = state.lookAt;
	mOldState.up = state.up;
	mOldState.zoomSpeed = state.zoomSpeed;
}


/**
* \brief load old state
*/
void CameraController::load()
{
	state.dist = mOldState.dist;
	state.theta = mOldState.theta;
	state.phi = mOldState.phi;
	state.oldX = mOldState.oldX;
	state.oldY = mOldState.oldY;
	state.pos = mOldState.pos;
	state.lookAt = mOldState.lookAt;
	state.up = mOldState.up;
	state.zoomSpeed = mOldState.zoomSpeed;
}


/**
* \brief Saves the current state to file
*/
void CameraController::saveToFile(const char* filename) const
{
	ofstream file(filename);

	if(file.fail()){ throw "Couldn't open file for saving current trackball."; }

	file << state.dist << " "
		<< state.theta << " "
		<< state.phi << " "
		<< state.oldX << " "
		<< state.oldY << " "
		<< state.pos[0] << " " << state.pos[1] << " " << state.pos[2] << " "
		<< state.lookAt[0] << " " << state.lookAt[1] << " " << state.lookAt[2] << " "
		<< state.up[0] << " " << state.up[1] << " " << state.up[2] << " "
		<< state.zoomSpeed;
}


/**
* \brief Load a state from file
*/
void CameraController::loadFromFile(const char* filename)
{
	ifstream file(filename);

	if(file.fail()){ throw "Couldn't open file for loading trackball."; }

	file >> state.dist
		>> state.theta
		>> state.phi
		>> state.oldX
		>> state.oldY
		>> state.pos[0] >> state.pos[1] >> state.pos[2]
	>> state.lookAt[0] >> state.lookAt[1] >> state.lookAt[2]
	>> state.up[0] >> state.up[1] >> state.up[2]
	>> state.zoomSpeed;
}


/**
* \brief Loads a goal target from file
*/
void CameraController::loadGoal(const char* filename)
{
	// load goal from file //
	ifstream file(filename);

	if(file.fail()){ throw "Couldn't open file for loading trackball."; }

	file >> mGoalState.dist
		>> mGoalState.theta
		>> mGoalState.phi
		>> mGoalState.oldX
		>> mGoalState.oldY
		>> mGoalState.pos[0] >> mGoalState.pos[1] >> mGoalState.pos[2]
	>> mGoalState.lookAt[0] >> mGoalState.lookAt[1] >> mGoalState.lookAt[2]
	>> mGoalState.up[0] >> mGoalState.up[1] >> mGoalState.up[2]
	>> mGoalState.zoomSpeed;

	// save old position //
	mOldState = state;
}


/**
* \brief increments the current values towards the goal
*/
void CameraController::interpolateToGoal(float t)
{
	float tm = 1.0f - t;

	state.dist     = tm * mOldState.dist     + t * mGoalState.dist;
	state.theta    = tm * mOldState.theta    + t * mGoalState.theta;
	state.phi      = tm * mOldState.phi      + t * mGoalState.phi;
	state.oldX     = tm * mOldState.oldX     + t * mGoalState.oldX;
	state.oldY     = tm * mOldState.oldY     + t * mGoalState.oldY;
	state.pos      = tm * mOldState.pos      + t * mGoalState.pos;
	state.lookAt   = tm * mOldState.lookAt   + t * mGoalState.lookAt;
	state.up       = tm * mOldState.up       + t * mGoalState.up;
	state.zoomSpeed= tm * mOldState.zoomSpeed+ t * mGoalState.zoomSpeed;
}
